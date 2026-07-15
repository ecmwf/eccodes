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
#include <cerrno>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

static const std::set<std::string> kConcepts = {"paramId", "shortName", "name", "units"};

struct Options {
    std::string definitions_root;
    bool debug_path_resolution = false;
    std::string columns;
    std::string scope;
    std::string name;
    std::string name_regex;
    std::string shortName;
    std::string shortName_regex;
    std::string paramId;
    std::string paramId_regex;
    std::string units;
    std::string units_regex;
    bool regex_case_sensitive = false;
    bool has_is_mtg2_switch = false;
    int is_mtg2_switch = 0;
    bool has_edition = false;
    int edition = 0;
    std::string attr;
    bool attr_strict = false;
    bool show_encoding = false;
    bool show_sources = false;
    std::string format = "line";
    bool no_truncate = false;
};

struct FieldMatcher {
    std::string wildcard;
    bool has_wildcard = false;
    std::string regex_text;
    bool has_regex = false;
    std::regex regex;
};

struct Record {
    int edition = 0;
    int sw = -1; // -1 means N/A for edition 1
    std::string scope;
    std::map<std::string, std::string> attrs;
    std::map<std::string, std::vector<std::string> > values;
    std::vector<std::string> sources;
};

static void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "codes_parameters: error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static void warningf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static std::string trim(const std::string& s)
{
    size_t b = 0;
    while (b < s.size() && isspace(static_cast<unsigned char>(s[b]))) b++;
    size_t e = s.size();
    while (e > b && isspace(static_cast<unsigned char>(s[e - 1]))) e--;
    return s.substr(b, e - b);
}

static bool starts_with(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

static bool ends_with(const std::string& s, const std::string& suffix)
{
    return string_ends_with(s.c_str(), suffix.c_str());
}

static std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> out;
    std::string part;
    std::stringstream ss(s);
    while (std::getline(ss, part, delim)) {
        part = trim(part);
        if (!part.empty()) out.push_back(part);
    }
    return out;
}

static char path_delimiter_char()
{
#ifdef ECCODES_ON_WINDOWS
    return ';';
#else
    return ':';
#endif
}

static void uniq(std::vector<std::string>& vals)
{
    std::set<std::string> seen;
    std::vector<std::string> out;
    for (size_t i = 0; i < vals.size(); ++i) {
        if (!seen.count(vals[i])) {
            seen.insert(vals[i]);
            out.push_back(vals[i]);
        }
    }
    vals.swap(out);
}

static bool is_dir(const std::string& path)
{
    return path_is_directory(path.c_str());
}

static bool is_file(const std::string& path)
{
    return path_is_regular_file(path.c_str());
}

static bool is_executable_file(const std::string& path)
{
    return is_file(path) && access(path.c_str(), X_OK) == 0;
}

static std::string dirname_of(const std::string& path)
{
    size_t p = path.find_last_of('/');
    if (p == std::string::npos) return ".";
    if (p == 0) return "/";
    return path.substr(0, p);
}

static std::string path_join(const std::string& a, const std::string& b)
{
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a[a.size() - 1] == '/') return a + b;
    return a + "/" + b;
}

static std::string shell_quote(const std::string& s)
{
    std::string q = "'";
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\'') q += "'\\''";
        else q += s[i];
    }
    q += "'";
    return q;
}

static std::string to_lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

static std::string best_suggestion(const std::string& key, const std::set<std::string>& known)
{
    int best_dist = INT_MAX;
    std::string best;
    for (std::set<std::string>::const_iterator it = known.begin(); it != known.end(); ++it) {
        int d = static_cast<int>(levenshteinDistance(key.c_str(), it->c_str()));
        if (d < best_dist) {
            best_dist = d;
            best = *it;
        }
    }
    if (best.empty()) return "";
    int max_dist = std::max(1, static_cast<int>(key.size() / 4));
    if (best_dist <= max_dist) return best;
    return "";
}

static bool wildcard_match_icase(const std::string& text, const std::string& pattern)
{
    std::string t = to_lower(text);
    std::string p = to_lower(pattern);

    size_t ti = 0, pi = 0, star = std::string::npos, match = 0;
    while (ti < t.size()) {
        if (pi < p.size() && (p[pi] == '?' || p[pi] == t[ti])) {
            pi++;
            ti++;
        }
        else if (pi < p.size() && p[pi] == '*') {
            star = pi++;
            match = ti;
        }
        else if (star != std::string::npos) {
            pi = star + 1;
            ti = ++match;
        }
        else {
            return false;
        }
    }
    while (pi < p.size() && p[pi] == '*') pi++;
    return pi == p.size();
}

static bool vector_contains(std::vector<std::string>& v, const std::string& x)
{
    return std::find(v.begin(), v.end(), x) != v.end();
}

static std::string format_values(const std::vector<std::string>& values)
{
    if (values.empty()) return "-";
    if (values.size() == 1) return values[0];
    std::string out = "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) out += ", ";
        out += values[i];
    }
    out += "]";
    return out;
}

static std::string format_attrs(const std::map<std::string, std::string>& attrs)
{
    if (attrs.empty()) return "-";
    std::string out;
    for (std::map<std::string, std::string>::const_iterator it = attrs.begin(); it != attrs.end(); ++it) {
        if (!out.empty()) out += ", ";
        out += it->first + "=" + it->second;
    }
    return out;
}

static std::string get_concept_from_filename(const std::string& filename)
{
    size_t p = filename.find('.');
    if (p == std::string::npos) return "";
    std::string base = filename.substr(0, p);
    if (kConcepts.count(base)) return base;
    return "";
}

static int infer_switch(const std::string& filename)
{
    if (ends_with(filename, ".lte33.def")) return 0;
    if (ends_with(filename, ".chemsplit.def")) return 2;
    if (ends_with(filename, ".def") && !ends_with(filename, ".legacy.def")) return 1;
    return -2;
}

static std::string unquote_value(std::string value)
{
    value = trim(value);
    if (value.size() >= 2 && value[0] == '\'' && value[value.size() - 1] == '\'') {
        std::string inner = value.substr(1, value.size() - 2);
        std::string out;
        for (size_t i = 0; i < inner.size(); ++i) {
            if (inner[i] == '\\' && i + 1 < inner.size() && inner[i + 1] == '\'') {
                out += '\'';
                i++;
            }
            else {
                out += inner[i];
            }
        }
        return out;
    }
    return value;
}

static std::string key_from_record(int edition, int sw, const std::string& scope, const std::map<std::string, std::string>& attrs)
{
    std::ostringstream oss;
    oss << edition << "|" << sw << "|" << scope << "|";
    for (std::map<std::string, std::string>::const_iterator it = attrs.begin(); it != attrs.end(); ++it) {
        oss << it->first << "=" << it->second << ";";
    }
    return oss.str();
}

static int add_record_value(
    std::map<std::string, Record>& records,
    int edition,
    int sw,
    const std::string& scope,
    const std::map<std::string, std::string>& attrs,
    const std::string& concept,
    const std::string& value,
    const std::string& source)
{
    std::string key = key_from_record(edition, sw, scope, attrs);
    if (!records.count(key)) {
        Record rec;
        rec.edition = edition;
        rec.sw = sw;
        rec.scope = scope;
        rec.attrs = attrs;
        records[key] = rec;
    }
    Record& rec = records[key];
    if (!vector_contains(rec.values[concept], value)) rec.values[concept].push_back(value);
    if (!vector_contains(rec.sources, source)) rec.sources.push_back(source);
    return 0;
}

static int parse_def_file(
    const std::string& file_path,
    const std::string& concept,
    int edition,
    int sw,
    const std::string& scope,
    const std::string& source,
    std::map<std::string, Record>& records)
{
    FILE* f = fopen(file_path.c_str(), "rb");
    if (!f) return errno;

    std::regex entry_re("^'((\\\\'|[^'])*)'\\s*=\\s*\\{\\s*$");
    std::regex attr_re("^([A-Za-z0-9_]+)\\s*=\\s*(.*?)\\s*;\\s*$");

    bool in_entry = false;
    std::string current_value;
    std::map<std::string, std::string> current_attrs;

    char line[8192];
    while (fgets(line, sizeof(line), f)) {
        std::string s = trim(line);
        if (s.empty() || s[0] == '#') continue;

        if (!in_entry) {
            std::smatch m;
            if (std::regex_match(s, m, entry_re)) {
                current_value = m[1].str();
                size_t pos = 0;
                while ((pos = current_value.find("\\\\'", pos)) != std::string::npos) {
                    current_value.replace(pos, 2, "'");
                    pos += 1;
                }
                current_attrs.clear();
                in_entry = true;
            }
            continue;
        }

        if (!s.empty() && s[0] == '}') {
            add_record_value(records, edition, sw, scope, current_attrs, concept, current_value, source);
            in_entry = false;
            current_value.clear();
            current_attrs.clear();
            continue;
        }

        std::smatch ma;
        if (std::regex_match(s, ma, attr_re)) {
            current_attrs[ma[1].str()] = unquote_value(ma[2].str());
        }
    }

    fclose(f);
    return 0;
}

static void scan_dir_recursive(const std::string& root, std::vector<std::string>& out_files)
{
    DIR* d = opendir(root.c_str());
    if (!d) return;

    struct dirent* ent = NULL;
    while ((ent = readdir(d)) != NULL) {
        const char* name = ent->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        std::string path = path_join(root, name);
        struct stat st;
        if (lstat(path.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            scan_dir_recursive(path, out_files);
        }
        else if (S_ISREG(st.st_mode)) {
            out_files.push_back(path);
        }
    }
    closedir(d);
}

static int scan_definitions(const std::string& definitions_root, std::map<std::string, Record>& records)
{
    std::vector<std::string> files;
    scan_dir_recursive(definitions_root, files);

    for (size_t i = 0; i < files.size(); ++i) {
        std::string file_path = files[i];
        std::string rel = file_path;
        if (starts_with(rel, definitions_root + "/")) rel = rel.substr(definitions_root.size() + 1);

        if (!(starts_with(rel, "grib1/") || starts_with(rel, "grib2/"))) continue;
        std::string filename = rel.substr(rel.find_last_of('/') + 1);

        bool concept_file = false;
        for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end(); ++c) {
            if (filename == *c + ".def" || filename == *c + ".lte33.def" || filename == *c + ".chemsplit.def") {
                concept_file = true;
                break;
            }
        }
        if (!concept_file) continue;

        std::string concept = get_concept_from_filename(filename);
        if (concept.empty()) continue;

        int edition = starts_with(rel, "grib1/") ? 1 : 2;
        int sw = infer_switch(filename);
        if (edition == 1) sw = -1;
        if (edition == 2 && sw == -2) continue;

        std::string scope;
        size_t slash = rel.find('/');
        size_t last_slash = rel.find_last_of('/');
        if (slash == std::string::npos || last_slash == std::string::npos || last_slash <= slash) {
            scope = starts_with(rel, "grib1/") ? "grib1" : "grib2";
        }
        else {
            scope = rel.substr(0, last_slash);
        }

        parse_def_file(file_path, concept, edition, sw, scope, rel, records);
    }

    return 0;
}

static std::string read_cmd_output(const std::string& cmd)
{
    std::string out;
    FILE* p = popen(cmd.c_str(), "r");
    if (!p) return out;
    char buf[4096];
    while (fgets(buf, sizeof(buf), p)) out += buf;
    pclose(p);
    return out;
}

static std::string resolve_path_like_shell(const std::string& p)
{
    if (p.empty()) return p;
    if (p[0] == '/') return p;
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) return p;
    return path_join(cwd, p);
}

static std::string find_in_path(const std::string& exe)
{
    const char* env_path = getenv("PATH");
    if (!env_path) return "";
    std::vector<std::string> dirs = split(env_path, ':');
    for (size_t i = 0; i < dirs.size(); ++i) {
        std::string p = path_join(dirs[i], exe);
        if (is_executable_file(p)) return p;
    }
    return "";
}

static std::string locate_codes_export_resource(const std::string& argv0)
{
    const char* env = getenv("ECCODES_EXPORT_RESOURCE");
    if (env && is_executable_file(env)) return env;

    char exe_real[PATH_MAX] = {0};
    if (realpath(argv0.c_str(), exe_real)) {
        std::string bin_dir = dirname_of(exe_real);
        std::string sibling = path_join(bin_dir, "codes_export_resource");
        if (is_executable_file(sibling)) return sibling;
    }

    std::string from_path = find_in_path("codes_export_resource");
    if (!from_path.empty()) return from_path;

    return "";
}

static std::string mk_temp_dir()
{
    char tmpl[] = "/tmp/codes_parameters_memfs_XXXXXX";
    char* p = mkdtemp(tmpl);
    if (!p) return "";
    return p;
}

static bool run_export(const std::string& exporter, const std::string& resource, const std::string& out_file)
{
    std::string out_dir = dirname_of(out_file);
    std::string cmd_mkdir = "mkdir -p " + shell_quote(out_dir);
    if (system(cmd_mkdir.c_str()) != 0) return false;

    std::string cmd = shell_quote(exporter) + " -d " + shell_quote(resource) + " " + shell_quote(out_file) + " >/dev/null 2>/dev/null";
    return system(cmd.c_str()) == 0;
}

static std::vector<std::string> find_referenced_paths_in_file(const std::string& file_path)
{
    std::vector<std::string> refs;
    FILE* f = fopen(file_path.c_str(), "rb");
    if (!f) return refs;

    std::regex re("\"([^\"\\\\]+\\.(def|table|tmpl|list|txt))\"");
    char line[8192];
    while (fgets(line, sizeof(line), f)) {
        std::string s = line;
        std::smatch m;
        while (std::regex_search(s, m, re)) {
            refs.push_back(m[1].str());
            s = m.suffix().str();
        }
    }
    fclose(f);
    uniq(refs);
    return refs;
}

static std::string materialize_memfs(const std::string& exporter)
{
    std::string temp_root = mk_temp_dir();
    if (temp_root.empty()) return "";

    std::string defs_root = path_join(temp_root, "definitions");
    std::vector<std::string> queue;

    // Main concept files
    const char* main_seeds[] = {
        "grib1/paramId.def", "grib1/shortName.def", "grib1/name.def", "grib1/units.def",
        "grib2/paramId.def", "grib2/shortName.def", "grib2/name.def", "grib2/units.def",
        "grib2/paramId.lte33.def", "grib2/shortName.lte33.def", "grib2/name.lte33.def", "grib2/units.lte33.def",
        "grib2/paramId.chemsplit.def", "grib2/shortName.chemsplit.def", "grib2/name.chemsplit.def", "grib2/units.chemsplit.def"
    };
    for (size_t i = 0; i < sizeof(main_seeds) / sizeof(main_seeds[0]); ++i) {
        queue.push_back(main_seeds[i]);
    }

    // localConcepts for grib1
    const char* grib1_local_concepts[] = {
        "ammc", "cerise", "cnmc", "destine", "destinegen1", "ecmf", "edzw", "efkl",
        "egrr", "eidb", "ekmi", "enmi", "era", "era6", "eswi", "fire", "hydro",
        "hydro_legacy", "kwbc", "lcgcr", "lfpw", "lfpw1", "lowm", "nextgems", "rjtd",
        "s2s", "sbsj", "tigge", "uerra", "wpmip"
    };
    const char* concepts[] = {"paramId", "shortName", "name", "units"};
    for (size_t i = 0; i < sizeof(grib1_local_concepts) / sizeof(grib1_local_concepts[0]); ++i) {
        for (size_t j = 0; j < sizeof(concepts) / sizeof(concepts[0]); ++j) {
            std::string path = std::string("grib1/localConcepts/") + grib1_local_concepts[i] + "/" + concepts[j] + ".def";
            queue.push_back(path);
        }
    }

    // localConcepts for grib2
    const char* grib2_local_concepts[] = {
        "cerise", "cnmc", "destine", "destinegen1", "ecmf", "edzw", "efkl",
        "egrr", "ekmi", "era", "era6", "eswi", "fire", "hydro", "hydro_legacy",
        "kwbc", "lcgcr", "lfpw", "lfpw1", "nextgems", "s2s", "tigge", "uerra", "wpmip"
    };
    for (size_t i = 0; i < sizeof(grib2_local_concepts) / sizeof(grib2_local_concepts[0]); ++i) {
        for (size_t j = 0; j < sizeof(concepts) / sizeof(concepts[0]); ++j) {
            std::string path = std::string("grib2/localConcepts/") + grib2_local_concepts[i] + "/" + concepts[j] + ".def";
            queue.push_back(path);
            // Also add lte33 and chemsplit variants for grib2
            path = std::string("grib2/localConcepts/") + grib2_local_concepts[i] + "/" + concepts[j] + ".lte33.def";
            queue.push_back(path);
            path = std::string("grib2/localConcepts/") + grib2_local_concepts[i] + "/" + concepts[j] + ".chemsplit.def";
            queue.push_back(path);
        }
    }

    std::set<std::string> seen;
    bool exported_any = false;

    while (!queue.empty()) {
        std::string res = queue.front();
        queue.erase(queue.begin());
        if (seen.count(res)) continue;
        seen.insert(res);

        std::string out_file = path_join(defs_root, res);
        if (!run_export(exporter, res, out_file)) continue;

        exported_any = true;
        std::vector<std::string> refs = find_referenced_paths_in_file(out_file);
        for (size_t i = 0; i < refs.size(); ++i) {
            if (!seen.count(refs[i])) queue.push_back(refs[i]);
        }
    }

    if (!exported_any) return "";
    return defs_root;
}

static std::vector<std::string> script_default_roots(const std::string& argv0)
{
    char exe_real[PATH_MAX] = {0};
    std::string exe = argv0;
    if (!realpath(argv0.c_str(), exe_real)) {
        ssize_t n = readlink("/proc/self/exe", exe_real, sizeof(exe_real) - 1);
        if (n > 0) {
            exe_real[n] = 0;
            exe = exe_real;
        }
        else {
            exe = resolve_path_like_shell(argv0);
        }
    }
    else {
        exe = exe_real;
    }

    std::string script_dir = dirname_of(exe);
    std::string prefix = dirname_of(script_dir);

    std::vector<std::string> candidates;
    candidates.push_back(path_join(path_join(prefix, "share"), "eccodes/definitions"));
    candidates.push_back(path_join(prefix, "definitions"));
    candidates.push_back(path_join(path_join(prefix, "share"), "definitions"));
    candidates.push_back(path_join(path_join(dirname_of(prefix), "share"), "eccodes/definitions"));
    candidates.push_back("definitions");
    uniq(candidates);
    return candidates;
}

static std::vector<std::string> explicit_roots_from_args_env(const Options& opt)
{
    std::vector<std::string> roots;
    const char sep = path_delimiter_char();
    if (!opt.definitions_root.empty()) {
        roots = split(opt.definitions_root, sep);
        uniq(roots);
        return roots;
    }

    const char* e = getenv("ECCODES_EXTRA_DEFINITION_PATH");
    if (e) {
        std::vector<std::string> v = split(e, sep);
        roots.insert(roots.end(), v.begin(), v.end());
    }
    e = getenv("ECCODES_EXTRA_DEFINITIONS");
    if (e) {
        std::vector<std::string> v = split(e, sep);
        roots.insert(roots.end(), v.begin(), v.end());
    }
    e = getenv("ECCODES_EXTRA_DEFINITONS");
    if (e) {
        std::vector<std::string> v = split(e, sep);
        roots.insert(roots.end(), v.begin(), v.end());
    }
    e = getenv("ECCODES_DEFINITION_PATH");
    if (e) {
        std::vector<std::string> v = split(e, sep);
        roots.insert(roots.end(), v.begin(), v.end());
    }
    uniq(roots);
    return roots;
}

static int parse_attr_filter(const std::string& text, std::map<std::string, std::string>& out, std::string& err)
{
    out.clear();
    if (text.empty()) return 0;

    std::vector<std::string> pairs = split(text, ',');
    if (pairs.empty()) {
        err = "--attr must contain at least one key=value pair";
        return -1;
    }

    for (size_t i = 0; i < pairs.size(); ++i) {
        std::string p = pairs[i];
        size_t eq = p.find('=');
        if (eq == std::string::npos) {
            err = "Invalid --attr pair '" + p + "'. Expected key=value";
            return -1;
        }
        std::string k = trim(p.substr(0, eq));
        std::string v = trim(p.substr(eq + 1));
        if (k.empty()) {
            err = "Invalid --attr pair '" + p + "'. Key cannot be empty";
            return -1;
        }
        if (out.count(k)) {
            err = "Duplicate key '" + k + "' in --attr";
            return -1;
        }
        out[k] = v;
    }
    return 0;
}

static std::string short_usage_text(const char* prog)
{
    std::ostringstream o;
    o << "usage: " << prog << " [-h] [--definitions-root DEFINITIONS_ROOT]\n"
      << "                        [--debug-path-resolution] [--columns COLUMNS]\n"
    << "                        [--scope SCOPE]\n"
      << "                        [--name NAME] [--name-regex NAME_REGEX]\n"
      << "                        [--shortName SHORTNAME]\n"
      << "                        [--shortName-regex SHORTNAME_REGEX]\n"
      << "                        [--paramId PARAMID] [--paramId-regex PARAMID_REGEX]\n"
      << "                        [--units UNITS] [--units-regex UNITS_REGEX]\n"
      << "                        [--attr ATTR] [--attr-strict] [--regex-case-sensitive]\n"
      << "                        [--is_mtg2_switch {0,1,2}] [--edition {1,2}]\n"
      << "                        [--show-encoding] [--show-sources]\n"
      << "                        [--format {line,table}] [--no-truncate]\n";
    return o.str();
}

static std::string usage_text(const char* prog)
{
    std::ostringstream o;
    o << "usage: " << prog << " [-h] [--definitions-root DEFINITIONS_ROOT]\n"
      << "                        [--debug-path-resolution] [--columns COLUMNS]\n"
    << "                        [--scope SCOPE]\n"
      << "                        [--name NAME] [--name-regex NAME_REGEX]\n"
      << "                        [--shortName SHORTNAME]\n"
      << "                        [--shortName-regex SHORTNAME_REGEX]\n"
      << "                        [--paramId PARAMID] [--paramId-regex PARAMID_REGEX]\n"
      << "                        [--units UNITS] [--units-regex UNITS_REGEX]\n"
      << "                        [--attr ATTR] [--attr-strict] [--regex-case-sensitive]\n"
      << "                        [--is_mtg2_switch {0,1,2}] [--edition {1,2}]\n"
      << "                        [--show-encoding] [--show-sources]\n"
      << "                        [--format {line,table}] [--no-truncate]\n"
      << "\n"
      << "Query parameter definitions in ecCodes concept files.\n"
      << "\n"
      << "options:\n"
      << "  -h, --help            show this help message and exit\n"
      << "  --definitions-root DEFINITIONS_ROOT\n"
      << "                        Definitions root directory. Accepts multiple paths\n"
      << "                        separated by path separator. If omitted, resolution\n"
      << "                        order is: --definitions-root;\n"
      << "                        ECCODES_EXTRA_DEFINITION_PATH (plus aliases\n"
      << "                        ECCODES_EXTRA_DEFINITIONS and\n"
      << "                        ECCODES_EXTRA_DEFINITONS); ECCODES_DEFINITION_PATH;\n"
      << "                        then defaults <prefix>/share/eccodes/definitions,\n"
      << "                        <prefix>/definitions, <prefix>/share/definitions,\n"
      << "                        <prefix>/../share/eccodes/definitions, ./definitions.\n"
      << "                        If none exist and no explicit roots were provided,\n"
      << "                        MEMFS is tried automatically.\n"
      << "  --debug-path-resolution\n"
      << "                        Print resolved definitions path candidates and\n"
      << "                        selected roots to stderr\n"
      << "  --columns COLUMNS     Comma-separated list of output columns to show. Valid\n"
      << "                        columns: edition,is_mtg2_switch,scope,paramId,shortName,\n"
      << "                        name,units,encoding,sources\n"
    << "  --scope SCOPE         Comma-separated list of exact scopes to include,\n"
    << "                        e.g. 'grib2,grib2/localConcepts/ecmf'\n"
      << "  --name NAME           Wildcard pattern for name, e.g. '*precipitation*'\n"
      << "  --name-regex NAME_REGEX\n"
      << "                        PCRE2-style regex for name\n"
      << "  --shortName SHORTNAME\n"
      << "                        Wildcard pattern for shortName, e.g. '10m*'\n"
      << "  --shortName-regex SHORTNAME_REGEX\n"
      << "                        PCRE2-style regex for shortName\n"
      << "  --paramId PARAMID     Wildcard pattern for paramId\n"
      << "  --paramId-regex PARAMID_REGEX\n"
      << "                        PCRE2-style regex for paramId\n"
      << "  --units UNITS         Wildcard pattern for units\n"
      << "  --units-regex UNITS_REGEX\n"
      << "                        PCRE2-style regex for units\n"
      << "  --attr ATTR           Filter by attribute key/value pairs as\n"
      << "                        key=value,key=value. Non-strict mode matches entries\n"
      << "                        containing at least these pairs\n"
      << "  --attr-strict         With --attr, require an exact attribute set match\n"
      << "                        (same keys and values)\n"
      << "  --regex-case-sensitive\n"
      << "                        Make regex searches case-sensitive. By default regex\n"
      << "                        searches are case-insensitive\n"
      << "  --is_mtg2_switch {0,1,2}\n"
      << "                        Restrict GRIB2 to lte33=0, def=1, chemsplit=2\n"
      << "  --edition {1,2}       Restrict to GRIB edition 1 or 2\n"
      << "  --show-encoding       Show the GRIB encoding constraints (same key/value\n"
      << "                        conditions)\n"
      << "  --show-sources        Show source definition files contributing to each\n"
      << "                        result\n"
      << "  --format {line,table}\n"
      << "                        Output format: line (default) or table\n"
      << "  --no-truncate         With --format table, do not truncate cells to\n"
      << "                        terminal width\n";
    return o.str();
}

static int parse_args(int argc, char** argv, Options& opt, std::string& err)
{
    if (argc == 1) return 0; // no arguments - show short usage in main

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-h" || a == "--help") {
            return 1; // explicit help request
        }

        std::string key = a;
        std::string val;
        size_t eq = a.find('=');
        if (eq != std::string::npos) {
            key = a.substr(0, eq);
            val = a.substr(eq + 1);
        }

        auto need_value = [&](const char* name) -> bool {
            if (!val.empty()) return true;
            if (i + 1 >= argc) {
                err = std::string("option requires an argument: ") + name;
                return false;
            }
            val = argv[++i];
            return true;
        };

        if (key == "--definitions-root") {
            if (!need_value("--definitions-root")) return -1;
            opt.definitions_root = val;
        }
        else if (key == "--debug-path-resolution") {
            opt.debug_path_resolution = true;
        }
        else if (key == "--columns") {
            if (!need_value("--columns")) return -1;
            opt.columns = val;
        }
        else if (key == "--scope") {
            if (!need_value("--scope")) return -1;
            opt.scope = val;
        }
        else if (key == "--name") {
            if (!need_value("--name")) return -1;
            opt.name = val;
        }
        else if (key == "--name-regex") {
            if (!need_value("--name-regex")) return -1;
            opt.name_regex = val;
        }
        else if (key == "--shortName") {
            if (!need_value("--shortName")) return -1;
            opt.shortName = val;
        }
        else if (key == "--shortName-regex") {
            if (!need_value("--shortName-regex")) return -1;
            opt.shortName_regex = val;
        }
        else if (key == "--paramId") {
            if (!need_value("--paramId")) return -1;
            opt.paramId = val;
        }
        else if (key == "--paramId-regex") {
            if (!need_value("--paramId-regex")) return -1;
            opt.paramId_regex = val;
        }
        else if (key == "--units") {
            if (!need_value("--units")) return -1;
            opt.units = val;
        }
        else if (key == "--units-regex") {
            if (!need_value("--units-regex")) return -1;
            opt.units_regex = val;
        }
        else if (key == "--attr") {
            if (!need_value("--attr")) return -1;
            opt.attr = val;
        }
        else if (key == "--attr-strict") {
            opt.attr_strict = true;
        }
        else if (key == "--regex-case-sensitive") {
            opt.regex_case_sensitive = true;
        }
        else if (key == "--is_mtg2_switch") {
            if (!need_value("--is_mtg2_switch")) return -1;
            opt.has_is_mtg2_switch = true;
            opt.is_mtg2_switch = atoi(val.c_str());
            if (opt.is_mtg2_switch < 0 || opt.is_mtg2_switch > 2) {
                err = "--is_mtg2_switch must be one of 0,1,2";
                return -1;
            }
        }
        else if (key == "--edition") {
            if (!need_value("--edition")) return -1;
            opt.has_edition = true;
            opt.edition = atoi(val.c_str());
            if (opt.edition != 1 && opt.edition != 2) {
                err = "--edition must be 1 or 2";
                return -1;
            }
        }
        else if (key == "--show-encoding") {
            opt.show_encoding = true;
        }
        else if (key == "--show-sources") {
            opt.show_sources = true;
        }
        else if (key == "--format") {
            if (!need_value("--format")) return -1;
            opt.format = val;
            if (!(opt.format == "line" || opt.format == "table")) {
                err = "--format must be one of: line, table";
                return -1;
            }
        }
        else if (key == "--no-truncate") {
            opt.no_truncate = true;
        }
        else {
            err = "unrecognized arguments: " + a;
            return -1;
        }
    }

    if (opt.attr_strict && opt.attr.empty()) {
        err = "--attr-strict requires --attr";
        return -1;
    }

    if (!opt.name.empty() && !opt.name_regex.empty()) {
        err = "Use either --name or --name-regex, not both";
        return -1;
    }
    if (!opt.shortName.empty() && !opt.shortName_regex.empty()) {
        err = "Use either --shortName or --shortName-regex, not both";
        return -1;
    }
    if (!opt.paramId.empty() && !opt.paramId_regex.empty()) {
        err = "Use either --paramId or --paramId-regex, not both";
        return -1;
    }
    if (!opt.units.empty() && !opt.units_regex.empty()) {
        err = "Use either --units or --units-regex, not both";
        return -1;
    }

    return 0;
}

static bool match_any(const std::vector<std::string>& values, const FieldMatcher& m)
{
    if (!m.has_wildcard && !m.has_regex) return true;
    for (size_t i = 0; i < values.size(); ++i) {
        const std::string& v = values[i];
        if (m.has_wildcard && wildcard_match_icase(v, m.wildcard)) return true;
        if (m.has_regex && std::regex_search(v, m.regex)) return true;
    }
    return false;
}

static std::vector<std::string> merge_values(const std::vector<std::vector<std::string> >& groups)
{
    std::vector<std::string> out;
    for (size_t i = 0; i < groups.size(); ++i) {
        for (size_t j = 0; j < groups[i].size(); ++j) {
            if (!vector_contains(out, groups[i][j])) out.push_back(groups[i][j]);
        }
    }
    return out;
}

static std::string table_value(const Record& rec, const std::string& key)
{
    if (key == "edition") return std::to_string(rec.edition);
    if (key == "is_mtg2_switch") return rec.sw < 0 ? "-" : std::to_string(rec.sw);
    if (key == "scope") return rec.scope;
    if (key == "paramId") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("paramId");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "shortName") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("shortName");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "name") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("name");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "units") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("units");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "encoding") return format_attrs(rec.attrs);
    if (key == "sources") {
        if (rec.sources.empty()) return "-";
        std::string out;
        for (size_t i = 0; i < rec.sources.size(); ++i) {
            if (i) out += ", ";
            out += rec.sources[i];
        }
        return out;
    }
    return "-";
}

static std::string truncate_cell(const std::string& s, size_t width)
{
    if (s.size() <= width) return s;
    if (width <= 3) return s.substr(0, width);
    return s.substr(0, width - 3) + "...";
}

int main(int argc, char** argv)
{
    Options opt;
    std::string arg_err;
    int parse_res = parse_args(argc, argv, opt, arg_err);
    if (parse_res == 1) {
        printf("%s", usage_text(argv[0]).c_str());
        return 0;
    }
    if (parse_res == 0 && argc == 1) {
        printf("%s", short_usage_text(argv[0]).c_str());
        printf("Use --help for full help message.\n");
        return 0;
    }
    if (parse_res < 0) {
        printf("%s", short_usage_text(argv[0]).c_str());
        errorf("%s", arg_err.c_str());
        return 2;
    }

    std::vector<std::string> selected_columns;
    if (!opt.columns.empty()) {
        selected_columns = split(opt.columns, ',');
        static const std::set<std::string> valid_columns = {
            "edition", "is_mtg2_switch", "scope", "paramId", "shortName", "name", "units", "encoding", "sources"
        };
        if (selected_columns.empty()) {
            errorf("--columns must contain at least one column name");
            return 2;
        }
        for (size_t i = 0; i < selected_columns.size(); ++i) {
            if (!valid_columns.count(selected_columns[i])) {
                errorf("Invalid column name: %s", selected_columns[i].c_str());
                return 2;
            }
        }
        uniq(selected_columns);
    }

    std::map<std::string, std::string> attr_filter;
    if (!opt.attr.empty()) {
        std::string e;
        if (parse_attr_filter(opt.attr, attr_filter, e) != 0) {
            errorf("%s", e.c_str());
            return 2;
        }
    }

    std::vector<std::string> explicit_roots = explicit_roots_from_args_env(opt);
    std::vector<std::string> candidates;
    if (!explicit_roots.empty()) candidates = explicit_roots;
    else candidates = script_default_roots(argv[0]);

    std::vector<std::string> filesystem_roots;
    std::vector<std::string> memfs_roots;

    for (size_t i = 0; i < candidates.size(); ++i) {
        const std::string& r = candidates[i];
        if (starts_with(r, "/MEMFS")) {
            memfs_roots.push_back(r);
            continue;
        }
        if (is_dir(r)) {
            filesystem_roots.push_back(r);
        }
        else if (std::find(explicit_roots.begin(), explicit_roots.end(), r) != explicit_roots.end()) {
            warningf("Ignoring non-directory definitions root: %s", r.c_str());
        }
    }

    grib_context* ctx = grib_context_get_default();
    const char* def_path = ctx->grib_definition_files_path;
    std::string detected_default = def_path ? def_path : "";

    if (explicit_roots.empty() && starts_with(detected_default, "/MEMFS")) {
        memfs_roots.insert(memfs_roots.begin(), detected_default);
        uniq(memfs_roots);
        filesystem_roots.clear();
    }

    bool auto_memfs = false;
    if (explicit_roots.empty() && filesystem_roots.empty() && memfs_roots.empty()) {
        memfs_roots.push_back("/MEMFS/definitions");
        auto_memfs = true;
    }

    if (opt.debug_path_resolution) {
        fprintf(stderr, "DEBUG: explicit roots:\n");
        if (explicit_roots.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < explicit_roots.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", explicit_roots[i].c_str());

        fprintf(stderr, "DEBUG: candidate roots:\n");
        for (size_t i = 0; i < candidates.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", candidates[i].c_str());

        fprintf(stderr, "DEBUG: filesystem roots:\n");
        if (filesystem_roots.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < filesystem_roots.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", filesystem_roots[i].c_str());

        fprintf(stderr, "DEBUG: memfs roots:\n");
        if (memfs_roots.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < memfs_roots.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", memfs_roots[i].c_str());

        fprintf(stderr, "DEBUG: codes_info default definitions path: %s\n", detected_default.empty() ? "<unavailable>" : detected_default.c_str());
        if (auto_memfs) fprintf(stderr, "DEBUG: auto memfs fallback enabled\n");
    }

    std::map<std::string, Record> records_by_key;
    bool used_filesystem_backend = false;
    bool used_memfs_backend = false;

    for (size_t i = 0; i < filesystem_roots.size(); ++i) {
        used_filesystem_backend = true;
        scan_definitions(filesystem_roots[i], records_by_key);
    }

    if (!memfs_roots.empty()) {
        std::string exporter = locate_codes_export_resource(argv[0]);
        if (opt.debug_path_resolution) {
            fprintf(stderr, "DEBUG: codes_export_resource=%s\n", exporter.empty() ? "<not found>" : exporter.c_str());
        }
        if (exporter.empty()) {
            warningf("MEMFS path requested but codes_export_resource was not found. Skipping MEMFS definitions.");
        }
        else {
            std::string memfs_root = materialize_memfs(exporter);
            if (memfs_root.empty()) {
                warningf("Failed to materialize definitions from MEMFS.");
            }
            else {
                used_memfs_backend = true;
                scan_definitions(memfs_root, records_by_key);
            }
        }
    }

    std::string effective_backend = "none";
    if (used_memfs_backend && used_filesystem_backend) effective_backend = "mixed";
    else if (used_memfs_backend) effective_backend = "memfs";
    else if (used_filesystem_backend) effective_backend = "filesystem";

    if (opt.debug_path_resolution) {
        fprintf(stderr, "DEBUG: effective backend: %s\n", effective_backend.c_str());
    }

    if (records_by_key.empty()) {
        errorf("No valid definitions roots were found. Provide --definitions-root, set ECCODES_DEFINITION_PATH/ECCODES_EXTRA_DEFINITION_PATH, or ensure MEMFS export is available.");
        return 2;
    }

    std::vector<Record> records;
    records.reserve(records_by_key.size());
    for (std::map<std::string, Record>::iterator it = records_by_key.begin(); it != records_by_key.end(); ++it) {
        records.push_back(it->second);
    }

    // Validate attr keys against known keys
    if (!attr_filter.empty()) {
        std::set<std::string> known;
        for (size_t i = 0; i < records.size(); ++i) {
            for (std::map<std::string, std::string>::const_iterator a = records[i].attrs.begin(); a != records[i].attrs.end(); ++a) {
                known.insert(a->first);
            }
        }
        for (std::map<std::string, std::string>::const_iterator a = attr_filter.begin(); a != attr_filter.end(); ++a) {
            if (!known.count(a->first)) {
                std::string suggestion = best_suggestion(a->first, known);
                if (!suggestion.empty()) {
                    errorf("Key '%s' seems to be misspelled. Did you mean '%s'?", a->first.c_str(), suggestion.c_str());
                }
                else {
                    errorf("Key '%s' seems to be misspelled.", a->first.c_str());
                }
                return 2;
            }
        }
    }

    std::map<std::string, FieldMatcher> matchers;
    auto build_matcher = [&](const std::string& wildcard, const std::string& re_text, const char* opt_name) -> bool {
        FieldMatcher fm;
        if (!wildcard.empty()) {
            fm.has_wildcard = true;
            fm.wildcard = wildcard;
        }
        if (!re_text.empty()) {
            fm.has_regex = true;
            fm.regex_text = re_text;
            try {
                std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;
                if (!opt.regex_case_sensitive) flags |= std::regex_constants::icase;
                fm.regex = std::regex(re_text, flags);
            }
            catch (const std::exception& e) {
                errorf("Invalid pattern for %s-regex: %s", opt_name, e.what());
                return false;
            }
        }
        matchers[opt_name] = fm;
        return true;
    };

    if (!build_matcher(opt.name, opt.name_regex, "name")) return 2;
    if (!build_matcher(opt.shortName, opt.shortName_regex, "shortName")) return 2;
    if (!build_matcher(opt.paramId, opt.paramId_regex, "paramId")) return 2;
    if (!build_matcher(opt.units, opt.units_regex, "units")) return 2;

    std::set<std::string> scope_filter;
    if (!opt.scope.empty()) {
        std::vector<std::string> scope_items = split(opt.scope, ',');
        if (scope_items.empty()) {
            errorf("--scope must contain at least one non-empty scope");
            return 2;
        }
        for (size_t i = 0; i < scope_items.size(); ++i) scope_filter.insert(scope_items[i]);
    }

    std::vector<Record> filtered;
    for (size_t i = 0; i < records.size(); ++i) {
        const Record& rec = records[i];
        if (!scope_filter.empty() && !scope_filter.count(rec.scope)) continue;
        if (opt.has_is_mtg2_switch) {
            if (rec.sw < 0 || rec.sw != opt.is_mtg2_switch) continue;
        }
        if (opt.has_edition && rec.edition != opt.edition) continue;

        std::vector<std::string> empty;
        const std::vector<std::string>& names = rec.values.count("name") ? rec.values.find("name")->second : empty;
        const std::vector<std::string>& shorts = rec.values.count("shortName") ? rec.values.find("shortName")->second : empty;
        const std::vector<std::string>& pids = rec.values.count("paramId") ? rec.values.find("paramId")->second : empty;
        const std::vector<std::string>& units = rec.values.count("units") ? rec.values.find("units")->second : empty;

        if (!match_any(names, matchers["name"])) continue;
        if (!match_any(shorts, matchers["shortName"])) continue;
        if (!match_any(pids, matchers["paramId"])) continue;
        if (!match_any(units, matchers["units"])) continue;

        if (!attr_filter.empty()) {
            if (opt.attr_strict) {
                if (rec.attrs != attr_filter) continue;
            }
            else {
                bool ok = true;
                for (std::map<std::string, std::string>::const_iterator a = attr_filter.begin(); a != attr_filter.end(); ++a) {
                    std::map<std::string, std::string>::const_iterator it = rec.attrs.find(a->first);
                    if (it == rec.attrs.end() || it->second != a->second) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) continue;
            }
        }

        filtered.push_back(rec);
    }

    std::sort(filtered.begin(), filtered.end(), [](const Record& a, const Record& b) {
        if (a.edition != b.edition) return a.edition < b.edition;
        if (a.sw != b.sw) return a.sw < b.sw;
        if (a.scope != b.scope) return a.scope < b.scope;
        std::string ap = a.values.count("paramId") && !a.values.find("paramId")->second.empty() ? a.values.find("paramId")->second[0] : "";
        std::string bp = b.values.count("paramId") && !b.values.find("paramId")->second.empty() ? b.values.find("paramId")->second[0] : "";
        return ap < bp;
    });

    bool keep_attrs = opt.show_encoding || !attr_filter.empty() || (!selected_columns.empty() && std::find(selected_columns.begin(), selected_columns.end(), "encoding") != selected_columns.end());
    if (!keep_attrs) {
        std::map<std::string, std::vector<Record> > groups;
        for (size_t i = 0; i < filtered.size(); ++i) {
            const Record& r = filtered[i];
            std::string key = std::to_string(r.edition) + "|" + std::to_string(r.sw) + "|" + r.scope + "|";
            key += table_value(r, "paramId") + "|" + table_value(r, "shortName") + "|" + table_value(r, "name") + "|" + table_value(r, "units");
            groups[key].push_back(r);
        }

        std::vector<Record> deduped;
        for (std::map<std::string, std::vector<Record> >::iterator g = groups.begin(); g != groups.end(); ++g) {
            const std::vector<Record>& grp = g->second;
            if (grp.size() == 1) {
                deduped.push_back(grp[0]);
                continue;
            }
            Record out = grp[0];
            out.attrs.clear();
            std::vector<std::vector<std::string> > pv, sv, nv, uv, srcv;
            for (size_t i = 0; i < grp.size(); ++i) {
                const Record& rr = grp[i];
                if (rr.values.count("paramId")) pv.push_back(rr.values.find("paramId")->second);
                if (rr.values.count("shortName")) sv.push_back(rr.values.find("shortName")->second);
                if (rr.values.count("name")) nv.push_back(rr.values.find("name")->second);
                if (rr.values.count("units")) uv.push_back(rr.values.find("units")->second);
                srcv.push_back(rr.sources);
            }
            out.values["paramId"] = merge_values(pv);
            out.values["shortName"] = merge_values(sv);
            out.values["name"] = merge_values(nv);
            out.values["units"] = merge_values(uv);
            out.sources = merge_values(srcv);
            deduped.push_back(out);
        }
        filtered.swap(deduped);
    }

    if (filtered.empty()) {
        printf("No matching parameters found.\n");
        return 1;
    }

    if (opt.format == "line") {
        if (!selected_columns.empty()) {
            for (size_t i = 0; i < filtered.size(); ++i) {
                const Record& r = filtered[i];
                std::string line;
                for (size_t c = 0; c < selected_columns.size(); ++c) {
                    if (c) line += " ";
                    line += selected_columns[c] + "=" + table_value(r, selected_columns[c]);
                }
                printf("%s\n", line.c_str());
            }
            return 0;
        }

        for (size_t i = 0; i < filtered.size(); ++i) {
            const Record& r = filtered[i];
            std::string line;
            line += "edition=" + std::to_string(r.edition) + " ";
            if (r.sw >= 0) line += "is_mtg2_switch=" + std::to_string(r.sw) + " ";
            line += "scope=" + r.scope + " ";
            line += "paramId=" + table_value(r, "paramId") + " ";
            line += "shortName=" + table_value(r, "shortName") + " ";
            line += "name=" + table_value(r, "name") + " ";
            line += "units=" + table_value(r, "units");
            printf("%s\n", line.c_str());

            if (opt.show_encoding && !r.attrs.empty()) {
                printf("  encoding: %s\n", format_attrs(r.attrs).c_str());
            }
            if (opt.show_sources) {
                printf("  sources: %s\n", table_value(r, "sources").c_str());
            }
        }
        return 0;
    }

    // table format
    std::vector<std::string> cols;
    if (!selected_columns.empty()) {
        cols = selected_columns;
    }
    else {
        bool include_switch = false;
        for (size_t i = 0; i < filtered.size(); ++i) {
            if (filtered[i].sw >= 0) {
                include_switch = true;
                break;
            }
        }
        cols.push_back("edition");
        if (include_switch) cols.push_back("is_mtg2_switch");
        cols.push_back("scope");
        cols.push_back("paramId");
        cols.push_back("shortName");
        cols.push_back("name");
        cols.push_back("units");
        if (opt.show_encoding) cols.push_back("encoding");
        if (opt.show_sources) cols.push_back("sources");
    }

    std::vector<std::vector<std::string> > rows;
    for (size_t i = 0; i < filtered.size(); ++i) {
        std::vector<std::string> row;
        for (size_t c = 0; c < cols.size(); ++c) {
            row.push_back(table_value(filtered[i], cols[c]));
        }
        rows.push_back(row);
    }

    std::vector<size_t> widths(cols.size(), 0);
    for (size_t c = 0; c < cols.size(); ++c) widths[c] = cols[c].size();
    for (size_t r = 0; r < rows.size(); ++r) {
        for (size_t c = 0; c < cols.size(); ++c) {
            widths[c] = std::max(widths[c], rows[r][c].size());
        }
    }

    if (!opt.no_truncate) {
        std::map<std::string, size_t> maxw;
        maxw["edition"] = 7;
        maxw["is_mtg2_switch"] = 14;
        maxw["scope"] = 28;
        maxw["paramId"] = 12;
        maxw["shortName"] = 16;
        maxw["name"] = 48;
        maxw["units"] = 20;
        maxw["encoding"] = 60;
        maxw["sources"] = 60;
        for (size_t c = 0; c < cols.size(); ++c) {
            if (maxw.count(cols[c])) widths[c] = std::min(widths[c], maxw[cols[c]]);
        }
    }

    for (size_t c = 0; c < cols.size(); ++c) {
        std::string h = cols[c];
        if (h.size() < widths[c]) h += std::string(widths[c] - h.size(), ' ');
        if (c) printf(" | ");
        printf("%s", h.c_str());
    }
    printf("\n");

    for (size_t c = 0; c < cols.size(); ++c) {
        if (c) printf("-+-");
        printf("%s", std::string(widths[c], '-').c_str());
    }
    printf("\n");

    for (size_t r = 0; r < rows.size(); ++r) {
        for (size_t c = 0; c < cols.size(); ++c) {
            std::string v = rows[r][c];
            if (!opt.no_truncate) v = truncate_cell(v, widths[c]);
            if (v.size() < widths[c]) v += std::string(widths[c] - v.size(), ' ');
            if (c) printf(" | ");
            printf("%s", v.c_str());
        }
        printf("\n");
    }

    return 0;
}
