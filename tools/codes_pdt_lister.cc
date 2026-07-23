/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fnmatch.h>
#include <getopt.h>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

static const char* COLOR_START = "\033[1;31m";
static const char* COLOR_END   = "\033[0m";

struct Options {
    std::string addkeys;
    std::string notInPDT;
    std::string definitions_path;
    bool list_keys = false;
    bool show_csv = false;
    std::string order_by;
    std::string order = "asc";
    std::string name_regex;
    std::string name_glob;
    std::string keys;
    std::string keys_regex;
    bool regex_case_sensitive = false;
    std::string colour_highlight = "0";
};

struct Matrix {
    std::vector<std::string> keys;
    std::unordered_map<std::string, size_t> key_to_index;
    std::vector<int> pdt_ids;
    std::vector<std::vector<unsigned char> > present;
};

static void usage(const char* prog)
{
    fprintf(stderr,
            "Usage: %s [options]\n\n"
            "Template listing mode:\n"
            "  -k, --k, --addkeys KEYS            Comma-separated keys that must be present\n"
            "  -n, --n, --notInPDT KEYS           Comma-separated keys that must NOT be present\n"
            "      --name GLOB                    Filter template names by glob (substring style)\n"
            "      --name-regex REGEX             Filter template names by regex\n\n"
            "Key listing mode:\n"
            "      --list-keys                    List all known matrix keys\n"
            "      --keys KEYS                    Comma-separated key-name glob patterns\n"
            "      --keys-regex REGEX             Regex filter on key names\n\n"
            "Common options:\n"
            "      --definitions-path PATH        Override definitions path\n"
            "      --show-csv                     Show full path to keys_in_PDTNS.csv and exit\n"
            "      --order-by {id,name,key}       Sort key (default: id for templates, key for keys)\n"
            "      --order {asc,desc}             Sort order (default: asc)\n"
            "      --regex-case-sensitive         Regex matching is case-sensitive\n"
            "      --colour-highlight {0,1}       Highlight matched parts (default: 0)\n"
            "  -h, --help                         Show this help\n",
            prog);
}

static std::string trim(const std::string& s)
{
    size_t i = 0;
    size_t j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

static std::string to_lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

static bool has_glob_chars(const std::string& s)
{
    return s.find_first_of("*?[") != std::string::npos;
}

static std::vector<std::string> split_csv_line(const std::string& line)
{
    std::vector<std::string> out;
    std::string cur;
    for (char ch : line) {
        if (ch == ',') {
            out.push_back(cur);
            cur.clear();
        }
        else {
            cur.push_back(ch);
        }
    }
    out.push_back(cur);
    return out;
}

static std::vector<std::string> split_list_unique_sorted(const std::string& s)
{
    std::set<std::string> uniq;
    std::string cur;
    for (char ch : s) {
        if (ch == ',') {
            std::string t = trim(cur);
            if (!t.empty()) uniq.insert(t);
            cur.clear();
        }
        else {
            cur.push_back(ch);
        }
    }
    std::string t = trim(cur);
    if (!t.empty()) uniq.insert(t);
    return std::vector<std::string>(uniq.begin(), uniq.end());
}

static bool read_line(FILE* f, std::string& out)
{
    out.clear();
    int ch = 0;
    while ((ch = std::fgetc(f)) != EOF) {
        if (ch == '\r') continue;
        if (ch == '\n') return true;
        out.push_back(static_cast<char>(ch));
    }
    return !out.empty();
}

static bool resolve_defs_path(grib_context* c, const std::string& relative, std::string& full)
{
    char* path = grib_context_full_defs_path(c, relative.c_str());
    if (!path) return false;
    full = path;
    return true;
}

static FILE* open_defs_file(grib_context* c, const std::string& relative, std::string* full_path = NULL)
{
    std::string full;
    if (!resolve_defs_path(c, relative, full)) return NULL;
    FILE* f = codes_fopen(full.c_str(), "r");
    if (!f) return NULL;
    if (full_path) *full_path = full;
    return f;
}

static int parse_int(const std::string& s, int* out)
{
    char* end = NULL;
    long v    = std::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0') return 0;
    *out = static_cast<int>(v);
    return 1;
}

static int load_matrix(grib_context* c, Matrix& m)
{
    FILE* f = open_defs_file(c, "grib2/keys_in_PDTNS.csv");
    if (!f) {
        fprintf(stderr, "Error: could not open definition resource 'grib2/keys_in_PDTNS.csv'\n");
        return 1;
    }

    bool header_loaded = false;
    std::string line;
    while (read_line(f, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> cols = split_csv_line(line);
        if (!header_loaded) {
            if (cols.empty()) {
                fclose(f);
                fprintf(stderr, "Error: invalid CSV header\n");
                return 1;
            }
            for (size_t i = 1; i < cols.size(); ++i) {
                std::string key = trim(cols[i]);
                if (!key.empty()) {
                    m.key_to_index[key] = m.keys.size();
                    m.keys.push_back(key);
                }
            }
            header_loaded = true;
            continue;
        }

        if (cols.empty()) continue;
        int pdtn = 0;
        if (!parse_int(trim(cols[0]), &pdtn)) continue;

        std::vector<unsigned char> row(m.keys.size(), 0);
        size_t n = std::min(m.keys.size(), cols.size() > 0 ? cols.size() - 1 : 0);
        for (size_t i = 0; i < n; ++i) {
            if (!trim(cols[i + 1]).empty()) row[i] = 1;
        }

        m.pdt_ids.push_back(pdtn);
        m.present.push_back(row);
    }

    fclose(f);

    if (!header_loaded) {
        fprintf(stderr, "Error: empty CSV resource 'grib2/keys_in_PDTNS.csv'\n");
        return 1;
    }
    return 0;
}

static std::map<std::string, std::string> load_alias_map(grib_context* c)
{
    std::map<std::string, std::string> aliases;
    FILE* f = open_defs_file(c, "grib2/keys_in_PDTNS_aliases.csv");
    if (!f) return aliases;

    std::string pending_alias;
    std::string line;
    while (read_line(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (!pending_alias.empty()) {
            aliases[pending_alias] = line;
            pending_alias.clear();
            continue;
        }

        std::vector<std::string> cols = split_csv_line(line);
        if (cols.size() < 2) continue;
        std::string alias = trim(cols[0]);
        std::string can   = trim(cols[1]);
        if (alias.empty()) continue;

        if (!can.empty()) {
            aliases[alias] = can;
        }
        else {
            pending_alias = alias;
        }
    }

    fclose(f);
    return aliases;
}

static void resolve_aliases(std::vector<std::string>& keys, const std::map<std::string, std::string>& alias_map)
{
    std::set<std::string> out;
    for (const auto& k : keys) {
        auto it = alias_map.find(k);
        out.insert(it != alias_map.end() ? it->second : k);
    }
    keys.assign(out.begin(), out.end());
}

static int find_latest_table_path(grib_context* c, std::string& full_path)
{
    for (int v = 255; v >= 0; --v) {
        char rel[128] = {0,};
        std::snprintf(rel, sizeof(rel), "grib2/tables/%d/4.0.table", v);
        if (resolve_defs_path(c, rel, full_path)) return 0;
    }
    return 1;
}

static std::map<int, std::string> load_pdt_names(grib_context* c)
{
    std::map<int, std::string> names;
    std::string table_path;
    if (find_latest_table_path(c, table_path) != 0) return names;

    FILE* f = codes_fopen(table_path.c_str(), "r");
    if (!f) return names;

    std::string line;
    while (read_line(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Format: <id> <id> <name...>
        size_t p1 = line.find_first_of(" \t");
        if (p1 == std::string::npos) continue;
        std::string s_id = trim(line.substr(0, p1));

        size_t p2 = line.find_first_not_of(" \t", p1);
        if (p2 == std::string::npos) continue;
        size_t p3 = line.find_first_of(" \t", p2);
        if (p3 == std::string::npos) continue;
        size_t p4 = line.find_first_not_of(" \t", p3);

        int id = 0;
        if (!parse_int(s_id, &id)) continue;
        std::string name = (p4 == std::string::npos) ? "" : trim(line.substr(p4));
        names[id]        = name;
    }

    fclose(f);
    return names;
}

static std::vector<std::string> glob_literal_chunks(const std::string& pattern)
{
    std::vector<std::string> chunks;
    std::string cur;
    bool in_class = false;

    for (char ch : pattern) {
        if (in_class) {
            if (ch == ']') in_class = false;
            continue;
        }
        if (ch == '*' || ch == '?') {
            if (!cur.empty()) {
                chunks.push_back(cur);
                cur.clear();
            }
            continue;
        }
        if (ch == '[') {
            if (!cur.empty()) {
                chunks.push_back(cur);
                cur.clear();
            }
            in_class = true;
            continue;
        }
        cur.push_back(ch);
    }
    if (!cur.empty()) chunks.push_back(cur);
    return chunks;
}

static std::vector<std::pair<size_t, size_t> > find_glob_spans(const std::string& text, const std::string& pattern, bool case_sensitive)
{
    std::vector<std::pair<size_t, size_t> > spans;
    std::vector<std::string> chunks = glob_literal_chunks(pattern);
    if (chunks.empty()) return spans;

    std::string text_cmp = case_sensitive ? text : to_lower(text);

    for (const auto& chunk : chunks) {
        std::string chunk_cmp = case_sensitive ? chunk : to_lower(chunk);
        size_t pos            = 0;
        while (true) {
            pos = text_cmp.find(chunk_cmp, pos);
            if (pos == std::string::npos) break;
            spans.push_back(std::make_pair(pos, pos + chunk.size()));
            pos += chunk.size();
        }
    }

    return spans;
}

static std::vector<std::pair<size_t, size_t> > find_regex_spans(const std::string& text, const std::regex& re)
{
    std::vector<std::pair<size_t, size_t> > spans;
    auto begin = std::sregex_iterator(text.begin(), text.end(), re);
    auto end   = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        size_t start = static_cast<size_t>(it->position());
        size_t len   = static_cast<size_t>(it->length());
        if (len > 0) spans.push_back(std::make_pair(start, start + len));
    }
    return spans;
}

static std::string highlight_spans(const std::string& text,
                                   std::vector<std::pair<size_t, size_t> > spans,
                                   bool enabled)
{
    if (!enabled || spans.empty()) return text;

    std::sort(spans.begin(), spans.end());
    std::vector<std::pair<size_t, size_t> > merged;
    merged.push_back(spans[0]);
    for (size_t i = 1; i < spans.size(); ++i) {
        if (spans[i].first <= merged.back().second) {
            merged.back().second = std::max(merged.back().second, spans[i].second);
        }
        else {
            merged.push_back(spans[i]);
        }
    }

    std::string out;
    size_t pos = 0;
    for (const auto& s : merged) {
        if (s.first > text.size()) break;
        if (s.first > pos) out.append(text.substr(pos, s.first - pos));
        size_t end = std::min(s.second, text.size());
        if (end > s.first) {
            out.append(COLOR_START);
            out.append(text.substr(s.first, end - s.first));
            out.append(COLOR_END);
        }
        pos = end;
    }
    if (pos < text.size()) out.append(text.substr(pos));
    return out;
}

static std::string join_keys(const std::vector<std::string>& keys)
{
    std::string out;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i) out.append(", ");
        out.append(keys[i]);
    }
    return out;
}

static int parse_options(int argc, char** argv, Options& o)
{
    static struct option long_opts[] = {
        {"help", no_argument, 0, 'h'},
        {"k", required_argument, 0, 'k'},
        {"addkeys", required_argument, 0, 'k'},
        {"n", required_argument, 0, 'n'},
        {"notInPDT", required_argument, 0, 'n'},
        {"definitions-path", required_argument, 0, 1000},
        {"list-keys", no_argument, 0, 1001},
        {"show-csv", no_argument, 0, 1002},
        {"order-by", required_argument, 0, 1003},
        {"order", required_argument, 0, 1004},
        {"name-regex", required_argument, 0, 1005},
        {"name", required_argument, 0, 1006},
        {"keys", required_argument, 0, 1007},
        {"keys-regex", required_argument, 0, 1008},
        {"regex-case-sensitive", no_argument, 0, 1009},
        {"colour-highlight", required_argument, 0, 1010},
        {0, 0, 0, 0}
    };

    int c = 0;
    int idx = 0;
    while ((c = getopt_long(argc, argv, "hk:n:", long_opts, &idx)) != -1) {
        switch (c) {
            case 'h':
                usage(argv[0]);
                return 1;
            case 'k':
                o.addkeys = optarg ? optarg : "";
                break;
            case 'n':
                o.notInPDT = optarg ? optarg : "";
                break;
            case 1000:
                o.definitions_path = optarg ? optarg : "";
                break;
            case 1001:
                o.list_keys = true;
                break;
            case 1002:
                o.show_csv = true;
                break;
            case 1003:
                o.order_by = optarg ? optarg : "";
                break;
            case 1004:
                o.order = optarg ? optarg : "";
                break;
            case 1005:
                o.name_regex = optarg ? optarg : "";
                break;
            case 1006:
                o.name_glob = optarg ? optarg : "";
                break;
            case 1007:
                o.keys = optarg ? optarg : "";
                break;
            case 1008:
                o.keys_regex = optarg ? optarg : "";
                break;
            case 1009:
                o.regex_case_sensitive = true;
                break;
            case 1010:
                o.colour_highlight = optarg ? optarg : "";
                break;
            default:
                return 0;
        }
    }

    if (!o.order_by.empty() && o.order_by != "id" && o.order_by != "name" && o.order_by != "key") {
        fprintf(stderr, "Error: --order-by must be one of: id, name, key\n");
        return 0;
    }
    if (o.order != "asc" && o.order != "desc") {
        fprintf(stderr, "Error: --order must be one of: asc, desc\n");
        return 0;
    }
    if (o.colour_highlight != "0" && o.colour_highlight != "1") {
        fprintf(stderr, "Error: --colour-highlight must be 0 or 1\n");
        return 0;
    }

    return 1;
}

int main(int argc, char** argv)
{
    Options opt;
    if (!parse_options(argc, argv, opt)) {
        usage(argv[0]);
        return 1;
    }

    grib_context* c = grib_context_get_default();
    if (!opt.definitions_path.empty()) {
        grib_context_set_definitions_path(c, opt.definitions_path.c_str());
    }

    std::string csv_full_path;
    if (!resolve_defs_path(c, "grib2/keys_in_PDTNS.csv", csv_full_path)) {
        fprintf(stderr, "Error: could not find definition resource 'grib2/keys_in_PDTNS.csv'\n");
        fprintf(stderr, "Set ECCODES_DEFINITION_PATH or use --definitions-path\n");
        return 1;
    }

    if (opt.show_csv) {
        printf("%s\n", csv_full_path.c_str());
        return 0;
    }

    Matrix matrix;
    if (load_matrix(c, matrix) != 0) return 1;

    std::map<std::string, std::string> alias_map = load_alias_map(c);
    std::map<int, std::string> pdt_names         = load_pdt_names(c);

    bool color_enabled = (opt.colour_highlight == "1");

    bool list_keys_mode = opt.list_keys || ((!opt.keys.empty() || !opt.keys_regex.empty()) && opt.addkeys.empty());

    if (!opt.order_by.empty() && opt.order_by == "key" && !list_keys_mode) {
        fprintf(stderr, "Error: --order-by key can only be used together with key listing mode.\n");
        return 1;
    }

    if ((!opt.keys.empty() || !opt.keys_regex.empty()) && !opt.addkeys.empty() && !opt.list_keys) {
        fprintf(stderr,
                "Error: --keys/--keys-regex are for key listing. "
                "Use them without -k/--addkeys, or add --list-keys.\n");
        return 1;
    }

    if (list_keys_mode) {
        if (opt.order_by == "id" || opt.order_by == "name") {
            fprintf(stderr, "Error: with --list-keys, --order-by can only be omitted or set to key.\n");
            return 1;
        }

        std::vector<std::string> keys_out = matrix.keys;
        std::vector<std::string> key_glob_patterns;

        if (!opt.keys.empty()) {
            std::vector<std::string> patterns = split_list_unique_sorted(opt.keys);
            std::vector<std::string> patterns_lower;
            for (const auto& p : patterns) {
                std::string expanded = has_glob_chars(p) ? p : ("*" + p + "*");
                key_glob_patterns.push_back(expanded);
                patterns_lower.push_back(to_lower(expanded));
            }

            std::vector<std::string> filtered;
            for (const auto& key : keys_out) {
                std::string k = to_lower(key);
                bool ok       = false;
                for (const auto& p : patterns_lower) {
                    if (fnmatch(p.c_str(), k.c_str(), 0) == 0) {
                        ok = true;
                        break;
                    }
                }
                if (ok) filtered.push_back(key);
            }
            keys_out.swap(filtered);
        }

        std::regex key_regex;
        bool key_regex_valid = false;
        if (!opt.keys_regex.empty()) {
            try {
                auto flags = std::regex_constants::ECMAScript;
                if (!opt.regex_case_sensitive) flags |= std::regex_constants::icase;
                key_regex       = std::regex(opt.keys_regex, flags);
                key_regex_valid = true;
            }
            catch (const std::regex_error& e) {
                fprintf(stderr, "Error: invalid --keys-regex regex: %s\n", e.what());
                return 1;
            }

            std::vector<std::string> filtered;
            for (const auto& key : keys_out) {
                if (std::regex_search(key, key_regex)) filtered.push_back(key);
            }
            keys_out.swap(filtered);
        }

        std::sort(keys_out.begin(), keys_out.end());
        if (opt.order == "desc") std::reverse(keys_out.begin(), keys_out.end());

        if (keys_out.empty()) {
            printf("No matching keys found.\n");
            return 0;
        }

        for (const auto& key : keys_out) {
            std::vector<std::pair<size_t, size_t> > spans;
            for (const auto& p : key_glob_patterns) {
                auto s = find_glob_spans(key, p, false);
                spans.insert(spans.end(), s.begin(), s.end());
            }
            if (key_regex_valid) {
                auto s = find_regex_spans(key, key_regex);
                spans.insert(spans.end(), s.begin(), s.end());
            }

            std::string out = highlight_spans(key, spans, color_enabled);
            printf("%s\n", out.c_str());
        }

        return 0;
    }

    std::string order_by = opt.order_by.empty() ? "id" : opt.order_by;

    bool has_template_filter = !opt.addkeys.empty() || !opt.notInPDT.empty() || !opt.name_glob.empty() || !opt.name_regex.empty();
    if (!has_template_filter) {
        fprintf(stderr,
                "Error: provide at least one template filter: "
                "-k/--addkeys, -n/--notInPDT, --name or --name-regex.\n");
        return 1;
    }

    std::vector<std::string> keys_in     = split_list_unique_sorted(opt.addkeys);
    std::vector<std::string> keys_not_in = split_list_unique_sorted(opt.notInPDT);

    if (!alias_map.empty()) {
        resolve_aliases(keys_in, alias_map);
        resolve_aliases(keys_not_in, alias_map);
    }

    if (std::find(keys_in.begin(), keys_in.end(), "discipline") != keys_in.end() ||
        std::find(keys_not_in.begin(), keys_not_in.end(), "discipline") != keys_not_in.end()) {
        fprintf(stderr, "Error: do not specify 'discipline'; it is not part of section 4.\n");
        return 1;
    }

    std::set<std::string> unknown;
    for (const auto& k : keys_in) {
        if (matrix.key_to_index.find(k) == matrix.key_to_index.end()) unknown.insert(k);
    }
    for (const auto& k : keys_not_in) {
        if (matrix.key_to_index.find(k) == matrix.key_to_index.end()) unknown.insert(k);
    }
    if (!unknown.empty()) {
        std::vector<std::string> u(unknown.begin(), unknown.end());
        fprintf(stderr, "Error: unknown key(s): %s\n", join_keys(u).c_str());
        return 1;
    }

    if (!keys_in.empty()) {
        printf("Searching for templates with these keys:\n");
        printf("  %s\n", join_keys(keys_in).c_str());
    }
    if (!keys_not_in.empty()) {
        printf("%s\n", keys_in.empty() ? "Searching for templates without these keys:" : "But without these keys:");
        printf("  %s\n", join_keys(keys_not_in).c_str());
    }
    if (!keys_in.empty() || !keys_not_in.empty()) printf("\n");

    std::regex name_regex;
    bool name_regex_valid = false;
    if (!opt.name_regex.empty()) {
        try {
            auto flags = std::regex_constants::ECMAScript;
            if (!opt.regex_case_sensitive) flags |= std::regex_constants::icase;
            name_regex       = std::regex(opt.name_regex, flags);
            name_regex_valid = true;
        }
        catch (const std::regex_error& e) {
            fprintf(stderr, "Error: invalid --name-regex regex: %s\n", e.what());
            return 1;
        }
    }

    std::string name_glob_pattern;
    if (!opt.name_glob.empty()) {
        name_glob_pattern = "*" + opt.name_glob + "*";
    }

    std::vector<int> results;
    for (size_t row = 0; row < matrix.pdt_ids.size(); ++row) {
        bool ok = true;

        for (const auto& k : keys_not_in) {
            size_t idx = matrix.key_to_index[k];
            if (matrix.present[row][idx]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        for (const auto& k : keys_in) {
            size_t idx = matrix.key_to_index[k];
            if (!matrix.present[row][idx]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        int pdtn = matrix.pdt_ids[row];
        std::string name;
        auto it = pdt_names.find(pdtn);
        if (it != pdt_names.end()) name = it->second;

        if (!name_glob_pattern.empty()) {
            if (fnmatch(name_glob_pattern.c_str(), name.c_str(), 0) != 0) continue;
        }
        if (name_regex_valid) {
            if (!std::regex_search(name, name_regex)) continue;
        }

        results.push_back(pdtn);
    }

    if (results.empty()) {
        printf("No matching templates found.\n");
        return 0;
    }

    auto desc = (opt.order == "desc");
    if (order_by == "name") {
        std::sort(results.begin(), results.end(), [&pdt_names](int a, int b) {
            std::string na, nb;
            auto ia = pdt_names.find(a);
            auto ib = pdt_names.find(b);
            if (ia != pdt_names.end()) na = ia->second;
            if (ib != pdt_names.end()) nb = ib->second;
            if (na == nb) return a < b;
            return na < nb;
        });
    }
    else {
        std::sort(results.begin(), results.end());
    }
    if (desc) std::reverse(results.begin(), results.end());

    for (int pdtn : results) {
        std::string name = "(name not found)";
        auto it = pdt_names.find(pdtn);
        if (it != pdt_names.end()) name = it->second;

        std::vector<std::pair<size_t, size_t> > spans;
        if (!name_glob_pattern.empty()) {
            auto s = find_glob_spans(name, opt.name_glob, false);
            spans.insert(spans.end(), s.begin(), s.end());
        }
        if (name_regex_valid) {
            auto s = find_regex_spans(name, name_regex);
            spans.insert(spans.end(), s.begin(), s.end());
        }

        std::string display_name = highlight_spans(name, spans, color_enabled);
        printf("%d\t%s\n", pdtn, display_name.c_str());
    }

    printf("\n");
    return 0;
}
