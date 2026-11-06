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
#include "expression/Functor.h"

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

static const std::set<std::string> kConcepts = {"chemId", "chemShortName", "chemName", "chemFormula"};

struct Options {
    std::string definitions_root;
    bool debug_path_resolution = false;
    std::string columns;
    std::string scope;
    std::string chemName;
    std::string name_regex;
    std::string chemShortName;
    std::string chemShortName_regex;
    std::string chemId;
    std::string chemId_regex;
    std::string chemFormula;
    std::string chemFormula_regex;
    bool regex_case_sensitive = false;
    std::string attr;
    bool attr_strict = false;
    std::string attrkey;
    std::string nattrkey;
    std::string nattr;
    bool show_encoding = false;
    bool show_sources = false;
    std::string format = "line";
    std::string order = "asc";
    std::vector<std::string> order_by_fields;
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
    fprintf(stderr, "grib_chems: error: ");
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

// Text form of a concept condition value, e.g. 128, -2, missing(), [102,2]
static std::string condition_value_text(const grib_concept_condition* cond)
{
    char buf[64];
    if (cond->iarray) {
        std::string out = "[";
        for (size_t i = 0; i < cond->iarray->n; ++i) {
            if (i) out += ",";
            snprintf(buf, sizeof(buf), "%ld", cond->iarray->v[i]);
            out += buf;
        }
        return out + "]";
    }

    const eccodes::Expression* e = cond->expression;
    if (!e) return "";
    const std::string cls = e->class_name();
    if (cls == "string") {
        int err = 0;
        const char* s = e->evaluate_string(NULL, NULL, NULL, &err);
        return (err == 0 && s) ? s : "";
    }
    if (cls == "functor") {
        const eccodes::expression::Functor* f = dynamic_cast<const eccodes::expression::Functor*>(e);
        return f ? std::string(const_cast<eccodes::expression::Functor*>(f)->name()) + "()" : "";
    }
    if (cls == "accessor") {
        return e->get_name();
    }
    // Constants (negative numbers are a unary minus applied to a constant)
    if (cls == "long" || cls == "double" || cls == "unop") {
        if (e->native_type(NULL) == GRIB_TYPE_DOUBLE) {
            double d = 0;
            if (e->evaluate_double(NULL, &d) == GRIB_SUCCESS) {
                snprintf(buf, sizeof(buf), "%g", d);
                return buf;
            }
        }
        else {
            long l = 0;
            if (e->evaluate_long(NULL, &l) == GRIB_SUCCESS) {
                snprintf(buf, sizeof(buf), "%ld", l);
                return buf;
            }
        }
    }
    return std::string("<") + cls + ">";
}

// Parse a concept file with the ecCodes parser itself (works for files on disk and in MEMFS)
static int parse_def_file(
    const std::string& file_path,
    const std::string& concept,
    int edition,
    int sw,
    const std::string& scope,
    const std::string& source,
    std::map<std::string, Record>& records)
{
    grib_context* ctx = grib_context_get_default();
    grib_concept_value* list = grib_parse_concept_file(ctx, file_path.c_str());
    if (!list) return GRIB_INVALID_FILE;

    // Entries in file order
    std::vector<grib_concept_value*> entries;
    for (grib_concept_value* v = list; v; v = v->next) entries.push_back(v);

    std::map<std::string, std::string> attrs;
    for (size_t i = 0; i < entries.size(); ++i) {
        attrs.clear();
        for (grib_concept_condition* cond = entries[i]->conditions; cond; cond = cond->next) {
            attrs[cond->name] = condition_value_text(cond);
        }
        add_record_value(records, edition, sw, scope, attrs, concept, entries[i]->name, source);
    }

    for (size_t i = 0; i < entries.size(); ++i) {
        for (grib_concept_condition* cond = entries[i]->conditions; cond; cond = cond->next) {
            if (cond->iarray) grib_iarray_delete(cond->iarray);
            cond->iarray = NULL;
        }
        grib_concept_value_delete(ctx, entries[i]);
    }
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
        // Use d_type where available: an lstat per entry is very slow on network filesystems
        bool is_dir_entry = false, is_reg_entry = false;
#ifdef _DIRENT_HAVE_D_TYPE
        if (ent->d_type != DT_UNKNOWN) {
            is_dir_entry = (ent->d_type == DT_DIR);
            is_reg_entry = (ent->d_type == DT_REG);
        }
        else
#endif
        {
            struct stat st;
            if (lstat(path.c_str(), &st) != 0) continue;
            is_dir_entry = S_ISDIR(st.st_mode);
            is_reg_entry = S_ISREG(st.st_mode);
        }
        if (is_dir_entry) {
            scan_dir_recursive(path, out_files);
        }
        else if (is_reg_entry) {
            out_files.push_back(path);
        }
    }
    closedir(d);
}

// Concept files which can exist in MEMFS. MEMFS cannot be listed, so the candidates
// are enumerated and tested for existence.
static std::vector<std::string> memfs_candidate_files()
{
    static const char* grib1_local_concepts[] = {
        "ammc", "cerise", "cnmc", "destine", "destinegen1", "ecmf", "edzw", "efkl",
        "egrr", "eidb", "ekmi", "enmi", "era", "era6", "eswi", "fire", "hydro",
        "hydro_legacy", "kwbc", "lcgcr", "lfpw", "lfpw1", "lowm", "nextgems", "rjtd",
        "s2s", "sbsj", "tigge", "uerra", "wpmip"
    };
    static const char* grib2_local_concepts[] = {
        "cerise", "cnmc", "destine", "destinegen1", "ecmf", "edzw", "efkl",
        "egrr", "ekmi", "era", "era6", "eswi", "fire", "hydro", "hydro_legacy",
        "kwbc", "lcgcr", "lfpw", "lfpw1", "nextgems", "s2s", "tigge", "uerra", "wpmip"
    };
    static const char* grib2_suffixes[] = {".def", ".lte33.def", ".chemsplit.def"};

    std::vector<std::string> dirs1(1, "grib1"), dirs2(1, "grib2");
    for (size_t i = 0; i < sizeof(grib1_local_concepts) / sizeof(grib1_local_concepts[0]); ++i)
        dirs1.push_back(std::string("grib1/localConcepts/") + grib1_local_concepts[i]);
    for (size_t i = 0; i < sizeof(grib2_local_concepts) / sizeof(grib2_local_concepts[0]); ++i)
        dirs2.push_back(std::string("grib2/localConcepts/") + grib2_local_concepts[i]);

    std::vector<std::string> out;
    for (size_t d = 0; d < dirs1.size(); ++d)
        for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end(); ++c)
            out.push_back(dirs1[d] + "/" + *c + ".def");
    for (size_t d = 0; d < dirs2.size(); ++d)
        for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end(); ++c)
            for (size_t s = 0; s < sizeof(grib2_suffixes) / sizeof(grib2_suffixes[0]); ++s)
                out.push_back(dirs2[d] + "/" + *c + grib2_suffixes[s]);
    return out;
}

// Scan one definitions root (a directory or a MEMFS root). As in ecCodes, a definition file
// is taken from the first root which contains it: files already taken from a previous root
// ('claimed') are skipped.
static int scan_definitions(const std::string& definitions_root, std::map<std::string, Record>& records,
                            std::set<std::string>& claimed, bool debug)
{
    const bool is_memfs_root = starts_with(definitions_root, "/MEMFS");
    std::string root_prefix = definitions_root;
    while (root_prefix.size() > 1 && root_prefix[root_prefix.size() - 1] == '/') {
        root_prefix.erase(root_prefix.size() - 1);
    }

    // Paths relative to the root
    std::vector<std::string> rel_files;
    if (is_memfs_root) {
        std::vector<std::string> candidates = memfs_candidate_files();
        for (size_t i = 0; i < candidates.size(); ++i) {
            if (codes_access(path_join(root_prefix, candidates[i]).c_str(), F_OK) == 0) {
                rel_files.push_back(candidates[i]);
            }
        }
    }
    else {
        // Concept files only live below grib1/ and grib2/, so do not walk the rest of the tree
        std::vector<std::string> files;
        scan_dir_recursive(path_join(definitions_root, "grib1"), files);
        scan_dir_recursive(path_join(definitions_root, "grib2"), files);
        for (size_t i = 0; i < files.size(); ++i) {
            if (starts_with(files[i], root_prefix + "/")) rel_files.push_back(files[i].substr(root_prefix.size() + 1));
        }
    }

    for (size_t i = 0; i < rel_files.size(); ++i) {
        const std::string& rel = rel_files[i];
        std::string file_path = path_join(root_prefix, rel);

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

        // MEMFS cannot be listed, so a concept file found on disk but missing from the
        // MEMFS candidate list would be silently ignored in a MEMFS build (tested in ctest)
        if (debug && !is_memfs_root) {
            static const std::vector<std::string> memfs_list = memfs_candidate_files();
            if (std::find(memfs_list.begin(), memfs_list.end(), rel) == memfs_list.end())
                fprintf(stderr, "DEBUG: concept file not in MEMFS candidate list: %s\n", rel.c_str());
        }

        std::string scope;
        size_t slash = rel.find('/');
        size_t last_slash = rel.find_last_of('/');
        if (slash == std::string::npos || last_slash == std::string::npos || last_slash <= slash) {
            scope = starts_with(rel, "grib1/") ? "grib1" : "grib2";
        }
        else {
            scope = rel.substr(0, last_slash);
        }

        if (!claimed.insert(rel).second) continue;
        parse_def_file(file_path, concept, edition, sw, scope, rel, records);
    }

    return 0;
}

static std::string resolve_path_like_shell(const std::string& p)
{
    if (p.empty()) return p;
    if (p[0] == '/') return p;
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) return p;
    return path_join(cwd, p);
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

// Definitions roots in priority order: --definitions-root if given, otherwise the
// definitions path of the ecCodes library itself, i.e. ECCODES_EXTRA_DEFINITION_PATH,
// then ECCODES_DEFINITION_PATH or the built-in default (which may be MEMFS).
static std::vector<std::string> definitions_roots(const Options& opt, grib_context* ctx, bool& from_option)
{
    const char sep = path_delimiter_char();
    from_option = !opt.definitions_root.empty();
    std::vector<std::string> roots;
    if (from_option) {
        roots = split(opt.definitions_root, sep);
    }
    else {
        const char* p = grib_definition_path(ctx);
        if (p) roots = split(p, sep);
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

static int parse_attrkey_filter(const std::string& text, std::set<std::string>& out, std::string& err)
{
    out.clear();
    if (text.empty()) return 0;

    std::vector<std::string> keys = split(text, ',');
    if (keys.empty()) {
        err = "--attrkey must contain at least one key";
        return -1;
    }

    for (size_t i = 0; i < keys.size(); ++i) {
        std::string k = keys[i];
        if (k.empty()) {
            err = "Invalid --attrkey: empty key";
            return -1;
        }
        if (k.find('=') != std::string::npos) {
            err = "Invalid --attrkey key '" + k + "'. Keys must not contain '='";
            return -1;
        }
        if (out.count(k)) {
            err = "Duplicate key '" + k + "' in --attrkey";
            return -1;
        }
        out.insert(k);
    }
    return 0;
}

static int parse_nattrkey_filter(const std::string& text, std::set<std::string>& out, std::string& err)
{
    out.clear();
    if (text.empty()) return 0;

    std::vector<std::string> keys = split(text, ',');
    if (keys.empty()) {
        err = "--nattrkey must contain at least one key";
        return -1;
    }

    for (size_t i = 0; i < keys.size(); ++i) {
        std::string k = keys[i];
        if (k.empty()) {
            err = "Invalid --nattrkey: empty key";
            return -1;
        }
        if (k.find('=') != std::string::npos) {
            err = "Invalid --nattrkey key '" + k + "'. Keys must not contain '='";
            return -1;
        }
        if (out.count(k)) {
            err = "Duplicate key '" + k + "' in --nattrkey";
            return -1;
        }
        out.insert(k);
    }
    return 0;
}

static int parse_nattr_filter(const std::string& text, std::map<std::string, std::string>& out, std::string& err)
{
    out.clear();
    if (text.empty()) return 0;

    std::vector<std::string> pairs = split(text, ',');
    if (pairs.empty()) {
        err = "--nattr must contain at least one key=value pair";
        return -1;
    }

    for (size_t i = 0; i < pairs.size(); ++i) {
        std::string p = pairs[i];
        size_t eq = p.find('=');
        if (eq == std::string::npos) {
            err = "Invalid --nattr pair '" + p + "'. Expected key=value";
            return -1;
        }
        std::string k = trim(p.substr(0, eq));
        std::string v = trim(p.substr(eq + 1));
        if (k.empty()) {
            err = "Invalid --nattr pair '" + p + "'. Key cannot be empty";
            return -1;
        }
        if (out.count(k)) {
            err = "Duplicate key '" + k + "' in --nattr";
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
      << "                        [--chemName CHEMNAME] [--chemName-regex CHEMNAME_REGEX]\n"
      << "                        [--chemShortName CHEMSHORTNAME]\n"
      << "                        [--chemShortName-regex CHEMSHORTNAME_REGEX]\n"
      << "                        [--chemId CHEMID] [--chemId-regex CHEMID_REGEX]\n"
            << "                        [--chemFormula CHEMFORMULA] [--chemFormula-regex CHEMFORMULA_REGEX]\n"
    << "                        [--attr ATTR] [--attr-strict] [--attrkey ATTRKEY] [--nattrkey NATTRKEY]\n"
      << "                        [--nattr NATTR] [--regex-case-sensitive]\n"
      << "                        [--show-encoding] [--show-sources]\n"
    << "                        [--format {line,table}] [--order {asc,desc}] [--order-by FIELD] [--no-truncate]\n";
    return o.str();
}

static std::string usage_text(const char* prog)
{
    std::ostringstream o;
    o << "usage: " << prog << " [-h] [--definitions-root DEFINITIONS_ROOT]\n"
      << "                        [--debug-path-resolution] [--columns COLUMNS]\n"
    << "                        [--scope SCOPE]\n"
      << "                        [--chemName CHEMNAME] [--chemName-regex CHEMNAME_REGEX]\n"
      << "                        [--chemShortName CHEMSHORTNAME]\n"
      << "                        [--chemShortName-regex CHEMSHORTNAME_REGEX]\n"
      << "                        [--chemId CHEMID] [--chemId-regex CHEMID_REGEX]\n"
            << "                        [--chemFormula CHEMFORMULA] [--chemFormula-regex CHEMFORMULA_REGEX]\n"
    << "                        [--attr ATTR] [--attr-strict] [--attrkey ATTRKEY] [--nattrkey NATTRKEY]\n"
      << "                        [--nattr NATTR] [--regex-case-sensitive]\n"
      << "                        [--show-encoding] [--show-sources]\n"
    << "                        [--format {line,table}] [--order {asc,desc}] [--order-by FIELD] [--no-truncate]\n"
      << "\n"
      << "Query chemical constituent definitions in ecCodes concept files.\n"
      << "\n"
      << "options:\n"
      << "  -h, --help            show this help message and exit\n"
      << "  --definitions-root DEFINITIONS_ROOT\n"
      << "                        Definitions root directory. Accepts multiple paths\n"
      << "                        separated by path separator. If omitted, the same\n"
      << "                        path as ecCodes is used: ECCODES_EXTRA_DEFINITION_PATH,\n"
      << "                        then ECCODES_DEFINITION_PATH or the built-in default\n"
      << "                        (which may be MEMFS). As in ecCodes, a definition\n"
      << "                        file present in several roots is taken from the first\n"
      << "                        root that has it\n"
      << "  --debug-path-resolution\n"
      << "                        Print resolved definitions path candidates and\n"
      << "                        selected roots to stderr. Also reports concept files\n"
      << "                        read from disk which a MEMFS build would not find\n"
      << "  --columns COLUMNS     Comma-separated list of output columns to show. Valid\n"
    << "                        columns: edition,scope,chemId,chemShortName,\n"
    << "                        chemName,chemFormula,encoding,sources\n"
    << "  --scope SCOPE         Comma-separated list of exact scopes to include,\n"
    << "                        e.g. 'grib2,grib2/localConcepts/ecmf'\n"
      << "  --chemName CHEMNAME   Wildcard pattern for chemName, e.g. '*oxide*'\n"
      << "  --chemName-regex CHEMNAME_REGEX\n"
      << "                        PCRE2-style regex for chemName\n"
      << "  --chemShortName CHEMSHORTNAME\n"
      << "                        Wildcard pattern for chemShortName, e.g. 'CH*'\n"
      << "  --chemShortName-regex CHEMSHORTNAME_REGEX\n"
      << "                        PCRE2-style regex for chemShortName\n"
      << "  --chemId CHEMID       Wildcard pattern for chemId\n"
      << "  --chemId-regex CHEMID_REGEX\n"
      << "                        PCRE2-style regex for chemId\n"
    << "  --chemFormula CHEMFORMULA\n"
    << "                        Wildcard pattern for chemFormula\n"
    << "  --chemFormula-regex CHEMFORMULA_REGEX\n"
    << "                        PCRE2-style regex for chemFormula\n"
      << "  --attr ATTR           Filter by attribute key/value pairs as\n"
      << "                        key=value,key=value. Non-strict mode matches entries\n"
      << "                        containing at least these pairs\n"
    << "  --attr-strict         With --attr and/or --attrkey, require an exact\n"
      << "                        match (same keys and values)\n"
      << "  --attrkey ATTRKEY     Filter by attribute key presence (comma-separated).\n"
      << "                        Keeps only entries that have all specified keys\n"
      << "  --nattrkey NATTRKEY   Exclude entries with specific attribute keys\n"
      << "                        (comma-separated list)\n"
      << "  --nattr NATTR         Exclude entries with specific attribute key/value\n"
      << "                        pairs as key=value,key=value\n"
      << "  --regex-case-sensitive\n"
      << "                        Make regex searches case-sensitive. By default regex\n"
      << "                        searches are case-insensitive\n"
      << "  --show-encoding       Show the GRIB encoding constraints (same key/value\n"
      << "                        conditions)\n"
      << "  --show-sources        Show source definition files contributing to each\n"
      << "                        result\n"
      << "  --format {line,table}\n"
      << "                        Output format: line (default) or table\n"
    << "  --order {asc,desc}    Sort order for results by edition/scope/chemId\n"
    << "                        ascending (default) or descending\n"
            << "  --order-by FIELD      Primary sort fields (comma-separated, in priority\n"
            << "                        order). Valid: edition,scope,chemId,\n"
            << "                        chemShortName,chemName,chemFormula\n"
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

        auto need_value = [&](const char* opt_name) -> bool {
            if (!val.empty()) return true;
            if (i + 1 >= argc) {
                err = std::string("option requires an argument: ") + opt_name;
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
        else if (key == "--chemName") {
            if (!need_value("--chemName")) return -1;
            opt.chemName = val;
        }
        else if (key == "--chemName-regex") {
            if (!need_value("--chemName-regex")) return -1;
            opt.name_regex = val;
        }
        else if (key == "--chemShortName") {
            if (!need_value("--chemShortName")) return -1;
            opt.chemShortName = val;
        }
        else if (key == "--chemShortName-regex") {
            if (!need_value("--chemShortName-regex")) return -1;
            opt.chemShortName_regex = val;
        }
        else if (key == "--chemId") {
            if (!need_value("--chemId")) return -1;
            opt.chemId = val;
        }
        else if (key == "--chemId-regex") {
            if (!need_value("--chemId-regex")) return -1;
            opt.chemId_regex = val;
        }
        else if (key == "--chemFormula") {
            if (!need_value("--chemFormula")) return -1;
            opt.chemFormula = val;
        }
        else if (key == "--chemFormula-regex") {
            if (!need_value("--chemFormula-regex")) return -1;
            opt.chemFormula_regex = val;
        }
        else if (key == "--attr") {
            if (!need_value("--attr")) return -1;
            opt.attr = val;
        }
        else if (key == "--attr-strict") {
            opt.attr_strict = true;
        }
        else if (key == "--attrkey") {
            if (!need_value("--attrkey")) return -1;
            opt.attrkey = val;
        }
        else if (key == "--nattrkey") {
            if (!need_value("--nattrkey")) return -1;
            opt.nattrkey = val;
        }
        else if (key == "--nattr") {
            if (!need_value("--nattr")) return -1;
            opt.nattr = val;
        }
        else if (key == "--regex-case-sensitive") {
            opt.regex_case_sensitive = true;
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
        else if (key == "--order") {
            if (!need_value("--order")) return -1;
            opt.order = val;
            if (!(opt.order == "asc" || opt.order == "desc")) {
                err = "--order must be one of: asc, desc";
                return -1;
            }
        }
        else if (key == "--order-by") {
            if (!need_value("--order-by")) return -1;
            static const std::set<std::string> valid_order_by = {
                "edition", "scope", "chemId", "chemShortName", "chemName", "chemFormula"
            };
            std::vector<std::string> fields = split(val, ',');
            if (fields.empty()) {
                err = "--order-by must contain at least one field";
                return -1;
            }
            for (size_t j = 0; j < fields.size(); ++j) {
                if (!valid_order_by.count(fields[j])) {
                    err = "--order-by must contain only: edition, scope, chemId, chemShortName, chemName, chemFormula";
                    return -1;
                }
            }
            opt.order_by_fields = fields;
        }
        else if (key == "--no-truncate") {
            opt.no_truncate = true;
        }
        else {
            err = "unrecognized arguments: " + a;
            return -1;
        }
    }

    if (opt.attr_strict && opt.attr.empty() && opt.attrkey.empty()) {
        err = "--attr-strict requires --attr and/or --attrkey";
        return -1;
    }

    if (!opt.chemName.empty() && !opt.name_regex.empty()) {
        err = "Use either --chemName or --chemName-regex, not both";
        return -1;
    }
    if (!opt.chemShortName.empty() && !opt.chemShortName_regex.empty()) {
        err = "Use either --chemShortName or --chemShortName-regex, not both";
        return -1;
    }
    if (!opt.chemId.empty() && !opt.chemId_regex.empty()) {
        err = "Use either --chemId or --chemId-regex, not both";
        return -1;
    }
    if (!opt.chemFormula.empty() && !opt.chemFormula_regex.empty()) {
        err = "Use either --chemFormula or --chemFormula-regex, not both";
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
    if (key == "chemId") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("chemId");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "chemShortName") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("chemShortName");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "chemName") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("chemName");
        return it == rec.values.end() ? "-" : format_values(it->second);
    }
    if (key == "chemFormula") {
        std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find("chemFormula");
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
            "edition", "scope", "chemId", "chemShortName", "chemName", "chemFormula", "encoding", "sources"
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

    std::set<std::string> attrkey_filter;
    if (!opt.attrkey.empty()) {
        std::string e;
        if (parse_attrkey_filter(opt.attrkey, attrkey_filter, e) != 0) {
            errorf("%s", e.c_str());
            return 2;
        }
    }

    std::set<std::string> nattrkey_filter;
    if (!opt.nattrkey.empty()) {
        std::string e;
        if (parse_nattrkey_filter(opt.nattrkey, nattrkey_filter, e) != 0) {
            errorf("%s", e.c_str());
            return 2;
        }
    }

    std::map<std::string, std::string> nattr_filter;
    if (!opt.nattr.empty()) {
        std::string e;
        if (parse_nattr_filter(opt.nattr, nattr_filter, e) != 0) {
            errorf("%s", e.c_str());
            return 2;
        }
    }

    grib_context* ctx = grib_context_get_default();
    const char* def_path = grib_definition_path(ctx);
    bool roots_from_option = false;
    std::vector<std::string> candidates = definitions_roots(opt, ctx, roots_from_option);

    // Usable roots in priority order (MEMFS or existing directories), without duplicates
    std::vector<std::string> roots;
    std::set<std::string> seen_real_roots;
    for (size_t i = 0; i < candidates.size(); ++i) {
        const std::string& r = candidates[i];
        std::string canonical = "/MEMFS"; // all MEMFS roots share one tree
        if (!starts_with(r, "/MEMFS")) {
            if (!is_dir(r)) {
                if (roots_from_option) warningf("Ignoring non-directory definitions root: %s", r.c_str());
                continue;
            }
            char real[PATH_MAX] = {0};
            canonical = realpath(r.c_str(), real) ? std::string(real) : r;
        }
        if (!seen_real_roots.insert(canonical).second) continue;
        roots.push_back(r);
    }

    // Fallback, e.g. for a relocated installation where the built-in path does not exist
    bool used_fallback = false;
    bool auto_memfs = false;
    if (roots.empty() && !roots_from_option) {
        std::vector<std::string> fallback = script_default_roots(argv[0]);
        for (size_t i = 0; i < fallback.size(); ++i) {
            if (is_dir(fallback[i])) {
                roots.push_back(fallback[i]);
                used_fallback = true;
                break;
            }
        }
        if (roots.empty()) {
            roots.push_back("/MEMFS/definitions");
            auto_memfs = true;
        }
    }

    if (opt.debug_path_resolution) {
        fprintf(stderr, "DEBUG: ecCodes definitions path: %s\n", def_path ? def_path : "<unavailable>");
        fprintf(stderr, "DEBUG: candidate roots (from %s):\n", roots_from_option ? "--definitions-root" : "ecCodes definitions path");
        if (candidates.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < candidates.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", candidates[i].c_str());

        fprintf(stderr, "DEBUG: selected roots (first match wins):\n");
        if (roots.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < roots.size(); ++i) fprintf(stderr, "DEBUG:   %s\n", roots[i].c_str());
        if (used_fallback) fprintf(stderr, "DEBUG: no usable root on the ecCodes path, using fallback next to the executable\n");
        if (auto_memfs) fprintf(stderr, "DEBUG: auto memfs fallback enabled\n");
    }

    std::map<std::string, Record> records_by_key;
    std::set<std::string> claimed_files;
    bool used_filesystem_backend = false;
    bool used_memfs_backend = false;

    for (size_t i = 0; i < roots.size(); ++i) {
        if (!starts_with(roots[i], "/MEMFS")) {
            used_filesystem_backend = true;
            scan_definitions(roots[i], records_by_key, claimed_files, opt.debug_path_resolution);
            continue;
        }

        // Read in-process: codes_access/codes_fopen serve /MEMFS paths from memory
        used_memfs_backend = true;
        scan_definitions(roots[i], records_by_key, claimed_files, opt.debug_path_resolution);
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

    if (!build_matcher(opt.chemName, opt.name_regex, "chemName")) return 2;
    if (!build_matcher(opt.chemShortName, opt.chemShortName_regex, "chemShortName")) return 2;
    if (!build_matcher(opt.chemId, opt.chemId_regex, "chemId")) return 2;
    if (!build_matcher(opt.chemFormula, opt.chemFormula_regex, "chemFormula")) return 2;

    std::set<std::string> scope_filter;
    if (!opt.scope.empty()) {
        std::vector<std::string> scope_items = split(opt.scope, ',');
        if (scope_items.empty()) {
            errorf("--scope must contain at least one non-empty scope");
            return 2;
        }
        for (size_t i = 0; i < scope_items.size(); ++i) scope_filter.insert(scope_items[i]);

        std::set<std::string> known_scopes;
        for (size_t i = 0; i < records.size(); ++i) known_scopes.insert(records[i].scope);

        auto scopes_preview = [&](const std::set<std::string>& scopes) -> std::string {
            std::string out;
            int count = 0;
            for (std::set<std::string>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
                if (count == 12) {
                    out += ", ...";
                    break;
                }
                if (!out.empty()) out += ", ";
                out += *it;
                count++;
            }
            return out;
        };
        const std::string known_scopes_text = scopes_preview(known_scopes);

        for (std::set<std::string>::const_iterator s = scope_filter.begin(); s != scope_filter.end(); ++s) {
            if (!known_scopes.count(*s)) {
                std::string suggestion = best_suggestion(*s, known_scopes);
                if (!suggestion.empty()) {
                    errorf("Scope '%s' is not defined. Did you mean '%s'? Available scopes: %s", s->c_str(), suggestion.c_str(), known_scopes_text.c_str());
                }
                else {
                    errorf("Scope '%s' is not defined. Available scopes: %s", s->c_str(), known_scopes_text.c_str());
                }
                return 2;
            }
        }
    }

    std::vector<Record> filtered;
    for (size_t i = 0; i < records.size(); ++i) {
        const Record& rec = records[i];
        if (!scope_filter.empty() && !scope_filter.count(rec.scope)) continue;

        std::vector<std::string> empty;
        const std::vector<std::string>& names = rec.values.count("chemName") ? rec.values.find("chemName")->second : empty;
        const std::vector<std::string>& shorts = rec.values.count("chemShortName") ? rec.values.find("chemShortName")->second : empty;
        const std::vector<std::string>& pids = rec.values.count("chemId") ? rec.values.find("chemId")->second : empty;
        const std::vector<std::string>& chemFormula = rec.values.count("chemFormula") ? rec.values.find("chemFormula")->second : empty;

        if (!match_any(names, matchers["chemName"])) continue;
        if (!match_any(shorts, matchers["chemShortName"])) continue;
        if (!match_any(pids, matchers["chemId"])) continue;
        if (!match_any(chemFormula, matchers["chemFormula"])) continue;

        if (opt.attr_strict && (!attr_filter.empty() || !attrkey_filter.empty())) {
            std::set<std::string> expected_keys;
            for (std::map<std::string, std::string>::const_iterator a = attr_filter.begin(); a != attr_filter.end(); ++a) {
                expected_keys.insert(a->first);
            }
            for (std::set<std::string>::const_iterator k = attrkey_filter.begin(); k != attrkey_filter.end(); ++k) {
                expected_keys.insert(*k);
            }

            if (rec.attrs.size() != expected_keys.size()) continue;

            bool keys_ok = true;
            for (std::set<std::string>::const_iterator k = expected_keys.begin(); k != expected_keys.end(); ++k) {
                std::map<std::string, std::string>::const_iterator it = rec.attrs.find(*k);
                if (it == rec.attrs.end()) {
                    keys_ok = false;
                    break;
                }
            }
            if (!keys_ok) continue;

            bool values_ok = true;
            for (std::map<std::string, std::string>::const_iterator a = attr_filter.begin(); a != attr_filter.end(); ++a) {
                std::map<std::string, std::string>::const_iterator it = rec.attrs.find(a->first);
                if (it == rec.attrs.end() || it->second != a->second) {
                    values_ok = false;
                    break;
                }
            }
            if (!values_ok) continue;
        }
        else if (!attr_filter.empty()) {
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

        // Include only records that have all keys in attrkey_filter
        if (!opt.attr_strict && !attrkey_filter.empty()) {
            bool all_present = true;
            for (std::set<std::string>::const_iterator k = attrkey_filter.begin(); k != attrkey_filter.end(); ++k) {
                std::map<std::string, std::string>::const_iterator it = rec.attrs.find(*k);
                if (it == rec.attrs.end()) {
                    all_present = false;
                    break;
                }
            }
            if (!all_present) continue;
        }

        // Exclude records matching nattrkey_filter (records that have any of the excluded keys)
        if (!nattrkey_filter.empty()) {
            bool has_excluded_key = false;
            for (std::set<std::string>::const_iterator k = nattrkey_filter.begin(); k != nattrkey_filter.end(); ++k) {
                if (rec.attrs.count(*k)) {
                    has_excluded_key = true;
                    break;
                }
            }
            if (has_excluded_key) continue;
        }

        // Exclude records matching nattr_filter (records that have any of the excluded key=value pairs)
        if (!nattr_filter.empty()) {
            bool has_excluded_pair = false;
            for (std::map<std::string, std::string>::const_iterator a = nattr_filter.begin(); a != nattr_filter.end(); ++a) {
                std::map<std::string, std::string>::const_iterator it = rec.attrs.find(a->first);
                if (it != rec.attrs.end() && it->second == a->second) {
                    has_excluded_pair = true;
                    break;
                }
            }
            if (has_excluded_pair) continue;
        }

        filtered.push_back(rec);
    }

    auto parse_id = [](const std::string& s) -> long long {
        if (s.empty()) return LLONG_MIN;
        char* end;
        long long v = strtoll(s.c_str(), &end, 10);
        return (end != s.c_str()) ? v : LLONG_MIN;
    };
    auto first_value = [](const Record& r, const std::string& key) -> std::string {
        std::map<std::string, std::vector<std::string> >::const_iterator it = r.values.find(key);
        if (it == r.values.end() || it->second.empty()) return "";
        return it->second[0];
    };

    auto cmp_ll = [](long long x, long long y) -> int {
        if (x < y) return -1;
        if (x > y) return 1;
        return 0;
    };
    auto cmp_str = [](const std::string& x, const std::string& y) -> int {
        if (x < y) return -1;
        if (x > y) return 1;
        return 0;
    };

    auto cmp_default = [&](const Record& a, const Record& b) -> int {
        int c = cmp_ll(a.edition, b.edition);
        if (c) return c;
        c = cmp_ll(a.sw, b.sw);
        if (c) return c;
        c = cmp_str(a.scope, b.scope);
        if (c) return c;
        return cmp_ll(parse_id(first_value(a, "chemId")), parse_id(first_value(b, "chemId")));
    };

    auto cmp_records = [&](const Record& a, const Record& b) -> int {
        int c = 0;
        for (size_t i = 0; i < opt.order_by_fields.size(); ++i) {
            const std::string& field = opt.order_by_fields[i];
            if (field == "edition") c = cmp_ll(a.edition, b.edition);
            else if (field == "scope") c = cmp_str(a.scope, b.scope);
            else if (field == "chemId") c = cmp_ll(parse_id(first_value(a, "chemId")), parse_id(first_value(b, "chemId")));
            else if (field == "chemShortName") c = cmp_str(first_value(a, "chemShortName"), first_value(b, "chemShortName"));
            else if (field == "chemName") c = cmp_str(first_value(a, "chemName"), first_value(b, "chemName"));
            else if (field == "chemFormula") c = cmp_str(first_value(a, "chemFormula"), first_value(b, "chemFormula"));
            if (c) return c;
        }
        return cmp_default(a, b);
    };

    auto record_before = [&cmp_records, &opt](const Record& a, const Record& b) {
        int c = cmp_records(a, b);
        return (opt.order == "desc") ? (c > 0) : (c < 0);
    };
    std::stable_sort(filtered.begin(), filtered.end(), record_before);

    bool keep_attrs = opt.show_encoding || !attr_filter.empty() || (!selected_columns.empty() && std::find(selected_columns.begin(), selected_columns.end(), "encoding") != selected_columns.end());
    if (!keep_attrs) {
        std::map<std::string, std::vector<Record> > groups;
        for (size_t i = 0; i < filtered.size(); ++i) {
            const Record& r = filtered[i];
            std::string key = std::to_string(r.edition) + "|" + std::to_string(r.sw) + "|" + r.scope + "|";
            key += table_value(r, "chemId") + "|" + table_value(r, "chemShortName") + "|" + table_value(r, "chemName") + "|" + table_value(r, "chemFormula");
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
                if (rr.values.count("chemId")) pv.push_back(rr.values.find("chemId")->second);
                if (rr.values.count("chemShortName")) sv.push_back(rr.values.find("chemShortName")->second);
                if (rr.values.count("chemName")) nv.push_back(rr.values.find("chemName")->second);
                if (rr.values.count("chemFormula")) uv.push_back(rr.values.find("chemFormula")->second);
                srcv.push_back(rr.sources);
            }
            out.values["chemId"] = merge_values(pv);
            out.values["chemShortName"] = merge_values(sv);
            out.values["chemName"] = merge_values(nv);
            out.values["chemFormula"] = merge_values(uv);
            out.sources = merge_values(srcv);
            deduped.push_back(out);
        }
        filtered.swap(deduped);

        std::stable_sort(filtered.begin(), filtered.end(), record_before);
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
            line += "scope=" + r.scope + " ";
            line += "chemId=" + table_value(r, "chemId") + " ";
            line += "chemShortName=" + table_value(r, "chemShortName") + " ";
            line += "chemName=" + table_value(r, "chemName") + " ";
            line += "chemFormula=" + table_value(r, "chemFormula");
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
        cols.push_back("edition");
        cols.push_back("scope");
        cols.push_back("chemId");
        cols.push_back("chemShortName");
        cols.push_back("chemName");
        cols.push_back("chemFormula");
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
        maxw["scope"] = 28;
        maxw["chemId"] = 12;
        maxw["chemShortName"] = 16;
        maxw["chemName"] = 48;
        maxw["chemFormula"] = 20;
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
