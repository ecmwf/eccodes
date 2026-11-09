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
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <sys/stat.h>
#include <vector>

static const std::set<std::string> kConcepts = { "paramId", "shortName", "name", "units" };

struct Options
{
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
    bool has_is_legacy = false;
    int is_legacy = 0;
    bool has_edition = false;
    int edition = 0;
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

struct FieldMatcher
{
    std::string wildcard;
    bool has_wildcard = false;
    std::string regex_text;
    bool has_regex = false;
    std::regex regex;
};

struct Record
{
    int edition = 0;
    int sw = -1;          // -1 means N/A for edition 1
    bool legacy = false;  // from a *.legacy.def file: last fallback in ecCodes
    std::string scope;
    std::map<std::string, std::string> attrs;
    std::map<std::string, std::vector<std::string> > values;
    std::vector<std::string> sources;
};

static void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "grib_parameters: error: ");
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
    while (b < s.size() && isspace(static_cast<unsigned char>(s[b])))
        b++;
    size_t e = s.size();
    while (e > b && isspace(static_cast<unsigned char>(s[e - 1])))
        e--;
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
        // '*' in the pattern is always a wildcard, also where the text has a literal '*'
        if (pi < p.size() && p[pi] != '*' && (p[pi] == '?' || p[pi] == t[ti])) {
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
    while (pi < p.size() && p[pi] == '*')
        pi++;
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

// Directories whose *.legacy.def files ecCodes reads (grib2/parameters.def: conceptsMasterDir and
// conceptsLocalDirECMF). They are the last fallback, after the files selected by the MTG2 switch,
// and apply to every MTG2 switch value.
static const char* kLegacyDirs[] = { "grib2", "grib2/localConcepts/ecmf" };
static const int kLegacySwitches[] = { 0, 1, 2 };

static bool is_legacy_concept_file(const std::string& rel)
{
    size_t slash = rel.find_last_of('/');
    if (slash == std::string::npos) return false;
    const std::string dir = rel.substr(0, slash);
    const std::string filename = rel.substr(slash + 1);
    bool dir_ok = false;
    for (size_t i = 0; i < sizeof(kLegacyDirs) / sizeof(kLegacyDirs[0]); ++i)
        if (dir == kLegacyDirs[i]) dir_ok = true;
    if (!dir_ok) return false;
    for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end(); ++c)
        if (filename == *c + ".legacy.def") return true;
    return false;
}

static std::string key_from_record(int edition, int sw, bool legacy, const std::string& scope, const std::map<std::string, std::string>& attrs)
{
    std::ostringstream oss;
    oss << edition << "|" << sw << "|" << legacy << "|" << scope << "|";
    for (std::map<std::string, std::string>::const_iterator it = attrs.begin(); it != attrs.end(); ++it) {
        oss << it->first << "=" << it->second << ";";
    }
    return oss.str();
}

static int add_record_value(
    std::map<std::string, Record>& records,
    int edition,
    int sw,
    bool legacy,
    const std::string& scope,
    const std::map<std::string, std::string>& attrs,
    const std::string& concept,
    const std::string& value,
    const std::string& source)
{
    std::string key = key_from_record(edition, sw, legacy, scope, attrs);
    if (!records.count(key)) {
        Record rec;
        rec.edition = edition;
        rec.sw = sw;
        rec.legacy = legacy;
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

// Whether a concept file has any content besides white space and '#' comments.
// Returns 1 if it has, 0 if not, -1 if it cannot be read. Uses codes_fopen, so works for MEMFS.
static int concept_file_has_content(const std::string& file_path)
{
    FILE* f = codes_fopen(file_path.c_str(), "r");
    if (!f) return -1;
    int c = 0, result = 0;
    bool in_comment = false;
    while ((c = fgetc(f)) != EOF) {
        if (in_comment) {
            if (c == '\n') in_comment = false;
        }
        else if (c == '#') {
            in_comment = true;
        }
        else if (!isspace(c)) {
            result = 1;
            break;
        }
    }
    if (ferror(f)) result = -1;
    fclose(f);
    return result;
}

static void free_concept_list(grib_context* ctx, grib_concept_value* list)
{
    while (list) {
        grib_concept_value* next = list->next;
        for (grib_concept_condition* cond = list->conditions; cond; cond = cond->next) {
            if (cond->iarray) grib_iarray_delete(cond->iarray);
            cond->iarray = NULL;
        }
        grib_concept_value_delete(ctx, list);
        list = next;
    }
}

// grib_parse_concept_file returns the result of the previous parse again when a file holds
// other definitions than concepts (e.g. "transient x = 1;"). To detect that, the last result
// is kept alive until a new one replaces it: a new result cannot reuse its address, and the
// parser never refers to freed memory. Freed by release_previous_concept_list() at the end.
static grib_concept_value* previous_concept_list = NULL;

static void release_previous_concept_list()
{
    free_concept_list(grib_context_get_default(), previous_concept_list);
    previous_concept_list = NULL;
}

// Parse a concept file with the ecCodes parser itself (works for files on disk and in MEMFS).
// A file without entries is valid; a file which cannot be read or parsed is an error.
static int parse_def_file(
    const std::string& file_path,
    const std::string& concept,
    int edition,
    const std::vector<int>& switches,
    bool legacy,
    const std::string& scope,
    const std::string& source,
    std::map<std::string, Record>& records)
{
    grib_context* ctx = grib_context_get_default();
    grib_concept_value* list = grib_parse_concept_file(ctx, file_path.c_str());
    if (list && list == previous_concept_list) list = NULL;  // stale result, see above
    if (list) release_previous_concept_list();               // the parser no longer refers to it
    if (!list) {
        // NULL is returned both for a file without entries and on failure: tell them apart
        const int content = concept_file_has_content(file_path);
        if (content == 0) return GRIB_SUCCESS;
        errorf("%s concept file %s", content < 0 ? "Cannot read" : "Cannot parse", file_path.c_str());
        return content < 0 ? GRIB_IO_PROBLEM : GRIB_INVALID_FILE;
    }

    // Entries in file order
    std::vector<grib_concept_value*> entries;
    for (grib_concept_value* v = list; v; v = v->next)
        entries.push_back(v);

    std::map<std::string, std::string> attrs;
    for (size_t i = 0; i < entries.size(); ++i) {
        attrs.clear();
        for (grib_concept_condition* cond = entries[i]->conditions; cond; cond = cond->next) {
            attrs[cond->name] = condition_value_text(cond);
        }
        for (size_t s = 0; s < switches.size(); ++s)
            add_record_value(records, edition, switches[s], legacy, scope, attrs, concept, entries[i]->name, source);
    }

    previous_concept_list = list;
    return 0;
}

// All regular files below root. As ecCodes opens files by path, symbolic links to files and
// directories are followed. Directories already on the current path (same device and inode)
// are not entered again, which protects against link cycles.
// A missing root is fine (e.g. no grib1/ in a definitions tree), but a directory which exists
// and cannot be listed is an error: it may hold overrides which ecCodes would open by name.
// Errors are reported and the walk continues, so that every failure is listed.
#ifndef ECCODES_ON_WINDOWS
static int scan_dir_recursive_impl(const std::string& root, std::vector<std::string>& out_files,
                                   std::vector<std::pair<dev_t, ino_t> >& ancestors)
{
    struct stat dst;
    if (stat(root.c_str(), &dst) != 0) {
        if (errno == ENOENT) return GRIB_SUCCESS;  // optional directory, or removed meanwhile
        errorf("Cannot access definitions directory %s: %s", root.c_str(), strerror(errno));
        return GRIB_IO_PROBLEM;
    }
    const std::pair<dev_t, ino_t> id(dst.st_dev, dst.st_ino);
    if (std::find(ancestors.begin(), ancestors.end(), id) != ancestors.end()) return GRIB_SUCCESS;  // link cycle

    DIR* d = opendir(root.c_str());
    if (!d) {
        errorf("Cannot list definitions directory %s: %s", root.c_str(), strerror(errno));
        return GRIB_IO_PROBLEM;
    }
    ancestors.push_back(id);

    int err = GRIB_SUCCESS;
    struct dirent* ent = NULL;
    while (true) {
        errno = 0;
        if ((ent = readdir(d)) == NULL) {
            if (errno != 0) {
                errorf("Cannot list definitions directory %s: %s", root.c_str(), strerror(errno));
                err = GRIB_IO_PROBLEM;
            }
            break;
        }
        const char* name = ent->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        std::string path = path_join(root, name);
        // Use d_type where available: a stat per entry is very slow on network filesystems.
        // Symbolic links and unknown types need a stat, which follows links.
        bool is_dir_entry = false, is_reg_entry = false;
    #ifdef _DIRENT_HAVE_D_TYPE
        if (ent->d_type == DT_DIR) {
            is_dir_entry = true;
        }
        else if (ent->d_type == DT_REG) {
            is_reg_entry = true;
        }
        else
    #endif
        {
            struct stat st;
            if (stat(path.c_str(), &st) != 0) {
                if (errno == ENOENT) continue;  // a dangling link
                errorf("Cannot access definitions entry %s: %s", path.c_str(), strerror(errno));
                err = GRIB_IO_PROBLEM;
                continue;
            }
            is_dir_entry = S_ISDIR(st.st_mode);
            is_reg_entry = S_ISREG(st.st_mode);
        }
        if (is_dir_entry) {
            if (scan_dir_recursive_impl(path, out_files, ancestors) != GRIB_SUCCESS) err = GRIB_IO_PROBLEM;
        }
        else if (is_reg_entry) {
            out_files.push_back(path);
        }
    }
    closedir(d);
    ancestors.pop_back();
    return err;
}

static int scan_dir_recursive(const std::string& root, std::vector<std::string>& out_files)
{
    std::vector<std::pair<dev_t, ino_t> > ancestors;
    return scan_dir_recursive_impl(root, out_files, ancestors);
}
#else
// Windows: no dirent.h, use _findfirst/_findnext from io.h. Directory links (symbolic links,
// junctions) are reported as directories and followed; there are no inodes to detect cycles,
// so the depth is limited instead (definitions are only a few levels deep).
static int scan_dir_recursive_impl(const std::string& root, std::vector<std::string>& out_files, int depth)
{
    if (depth > 32) return GRIB_SUCCESS;
    struct _finddata_t fd;
    intptr_t h = _findfirst(path_join(root, "*").c_str(), &fd);
    if (h == -1) {
        if (errno == ENOENT) return GRIB_SUCCESS;  // optional directory
        errorf("Cannot list definitions directory %s: %s", root.c_str(), strerror(errno));
        return GRIB_IO_PROBLEM;
    }
    int err = GRIB_SUCCESS;
    do {
        if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0) continue;
        std::string path = path_join(root, fd.name);
        if (fd.attrib & _A_SUBDIR) {
            if (scan_dir_recursive_impl(path, out_files, depth + 1) != GRIB_SUCCESS) err = GRIB_IO_PROBLEM;
        }
        else {
            out_files.push_back(path);
        }
    } while (_findnext(h, &fd) == 0);
    if (errno != ENOENT) {  // _findnext ends with ENOENT when there are no more entries
        errorf("Cannot list definitions directory %s: %s", root.c_str(), strerror(errno));
        err = GRIB_IO_PROBLEM;
    }
    _findclose(h);
    return err;
}

static int scan_dir_recursive(const std::string& root, std::vector<std::string>& out_files)
{
    return scan_dir_recursive_impl(root, out_files, 0);
}
#endif

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
    static const char* grib2_suffixes[] = { ".def", ".lte33.def", ".chemsplit.def" };

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
    for (size_t d = 0; d < sizeof(kLegacyDirs) / sizeof(kLegacyDirs[0]); ++d)
        for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end(); ++c)
            out.push_back(std::string(kLegacyDirs[d]) + "/" + *c + ".legacy.def");
    return out;
}

// The concept files a query needs. Edition, MTG2 switch, legacy flag and scope are the same
// for all entries of a file and part of the record key, so a file excluded by these filters
// cannot contribute to any result and need not be parsed (parsing dominates the run time).
struct FileFilter
{
    bool has_edition = false;
    int edition = 0;
    bool has_is_mtg2_switch = false;
    int is_mtg2_switch = 0;
    bool has_is_legacy = false;
    int is_legacy = 0;
    std::set<std::string> scopes;  // empty: all

    bool active() const { return has_edition || has_is_mtg2_switch || has_is_legacy || !scopes.empty(); }

    // Same conditions as the record filter in main()
    bool wants(int ed, const std::vector<int>& switches, bool legacy, const std::string& scope) const
    {
        if (has_edition && ed != edition) return false;
        if (has_is_mtg2_switch && std::find(switches.begin(), switches.end(), is_mtg2_switch) == switches.end()) return false;
        if (has_is_legacy && (ed == 1 || (legacy ? 1 : 0) != is_legacy)) return false;
        if (!scopes.empty() && !scopes.count(scope)) return false;
        return true;
    }
};

// Classification of a concept file by its path relative to a definitions directory
struct ConceptFile
{
    std::string concept;
    int edition = 0;
    std::vector<int> switches;  // the MTG2 switch values it applies to; -1 for edition 1
    bool legacy = false;
    std::string scope;
};

// False if rel is not a concept file the tool reads
static bool classify_concept_file(const std::string& rel, ConceptFile& cf)
{
    const std::string filename = rel.substr(rel.find_last_of('/') + 1);
    cf.legacy = is_legacy_concept_file(rel);
    bool concept_file = cf.legacy;
    for (std::set<std::string>::const_iterator c = kConcepts.begin(); c != kConcepts.end() && !concept_file; ++c) {
        if (filename == *c + ".def" || filename == *c + ".lte33.def" || filename == *c + ".chemsplit.def") {
            concept_file = true;
        }
    }
    if (!concept_file) return false;

    cf.concept = get_concept_from_filename(filename);
    if (cf.concept.empty()) return false;

    cf.edition = starts_with(rel, "grib1/") ? 1 : 2;
    int sw = infer_switch(filename);
    if (cf.edition == 1) sw = -1;
    if (cf.edition == 2 && sw == -2 && !cf.legacy) return false;
    cf.switches.assign(1, sw);
    if (cf.legacy) cf.switches.assign(kLegacySwitches, kLegacySwitches + sizeof(kLegacySwitches) / sizeof(kLegacySwitches[0]));

    const size_t slash = rel.find('/');
    const size_t last_slash = rel.find_last_of('/');
    if (slash == std::string::npos || last_slash == std::string::npos || last_slash <= slash) {
        cf.scope = starts_with(rel, "grib1/") ? "grib1" : "grib2";
    }
    else {
        cf.scope = rel.substr(0, last_slash);
    }
    return true;
}

// The directories of the definitions path in priority order, as ecCodes itself has split
// and resolved them
static std::vector<std::string> definitions_dirs(grib_context* ctx)
{
    // ecCodes sets up the directory list when it first looks up a definition file;
    // the file need not exist
    grib_context_full_defs_path(ctx, "boot.def");
    std::vector<std::string> dirs;
    for (const grib_string_list* d = ctx->grib_definition_files_dir; d; d = d->next) {
        if (d->value && !vector_contains(dirs, d->value)) dirs.push_back(d->value);
    }
    return dirs;
}

// Add the concept files of one definitions directory, as paths relative to it. MEMFS cannot
// be listed, so there the known candidates are looked up. The directory walk continues after
// an error, so that every failure is reported.
static int list_concept_files(const std::string& dir, std::set<std::string>& rel_files, bool debug)
{
    if (starts_with(dir, "/MEMFS")) {
        // Joined as grib_context_full_defs_path joins them, so that exactly the files are
        // listed which ecCodes finds (MEMFS paths are not normalised)
        const std::vector<std::string> candidates = memfs_candidate_files();
        for (size_t i = 0; i < candidates.size(); ++i) {
            if (codes_access((dir + "/" + candidates[i]).c_str(), F_OK) == 0) rel_files.insert(candidates[i]);
        }
        return GRIB_SUCCESS;
    }

    int err = GRIB_SUCCESS;
    std::string root = dir;
    while (root.size() > 1 && root[root.size() - 1] == '/')
        root.erase(root.size() - 1);
    // Concept files only live below grib1/ and grib2/, so do not walk the rest of the tree
    std::vector<std::string> files;
    if (scan_dir_recursive(path_join(root, "grib1"), files) != GRIB_SUCCESS) err = GRIB_IO_PROBLEM;
    if (scan_dir_recursive(path_join(root, "grib2"), files) != GRIB_SUCCESS) err = GRIB_IO_PROBLEM;

    const std::string prefix = (root == "/") ? root : root + "/";
    for (size_t i = 0; i < files.size(); ++i) {
        if (!starts_with(files[i], prefix)) continue;
        const std::string rel = files[i].substr(prefix.size());
        ConceptFile cf;
        if (!classify_concept_file(rel, cf)) continue;
        // MEMFS cannot be listed, so a concept file found on disk but missing from the
        // MEMFS candidate list would be silently ignored in a MEMFS build (tested in ctest)
        if (debug) {
            static const std::vector<std::string> memfs_list = memfs_candidate_files();
            if (std::find(memfs_list.begin(), memfs_list.end(), rel) == memfs_list.end())
                fprintf(stderr, "DEBUG: concept file not in MEMFS candidate list: %s\n", rel.c_str());
        }
        rel_files.insert(rel);
    }
    return err;
}

// Parse the concept files, in path order. Each one is read from where ecCodes takes it:
// grib_context_full_defs_path returns it from the first directory of the definitions path
// which has it. With a filter, only the files it wants are parsed.
static int parse_concept_files(grib_context* ctx, const std::set<std::string>& rel_files, std::map<std::string, Record>& records,
                               const FileFilter* filter, size_t& n_parsed, size_t& n_skipped)
{
    int err = GRIB_SUCCESS;  // all files are parsed, so that every failure is reported
    for (std::set<std::string>::const_iterator it = rel_files.begin(); it != rel_files.end(); ++it) {
        const std::string& rel = *it;
        ConceptFile cf;
        if (!classify_concept_file(rel, cf)) continue;
        if (filter && !filter->wants(cf.edition, cf.switches, cf.legacy, cf.scope)) {
            n_skipped++;
            continue;
        }
        const char* full = grib_context_full_defs_path(ctx, rel.c_str());
        if (!full) {  // listed, but gone meanwhile
            errorf("Cannot find concept file %s", rel.c_str());
            err = GRIB_IO_PROBLEM;
            continue;
        }
        n_parsed++;
        const int perr = parse_def_file(full, cf.concept, cf.edition, cf.switches, cf.legacy, cf.scope, rel, records);
        if (perr != GRIB_SUCCESS) err = perr;
    }
    return err;
}

// Split a key=value list at commas outside array brackets, so that values such as [102,2]
// stay whole. Pieces are trimmed and empty pieces dropped. Fails on unbalanced brackets.
static bool split_outside_brackets(const std::string& s, std::vector<std::string>& out)
{
    out.clear();
    int depth = 0;
    std::string part;
    for (size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '[') depth++;
        if (c == ']' && --depth < 0) return false;
        if (c == ',' && depth == 0) {
            part = trim(part);
            if (!part.empty()) out.push_back(part);
            part.clear();
        }
        else {
            part += c;
        }
    }
    if (depth != 0) return false;
    part = trim(part);
    if (!part.empty()) out.push_back(part);
    return true;
}

// Parse a key=value,key=value list for option opt_name (--attr, --nattr). Array values are
// written without spaces, as --show-encoding prints them: [102, 2] is the same as [102,2].
static int parse_key_value_filter(const std::string& opt_name, const std::string& text,
                                  std::map<std::string, std::string>& out, std::string& err)
{
    out.clear();
    if (text.empty()) return 0;

    std::vector<std::string> pairs;
    if (!split_outside_brackets(text, pairs)) {
        err = "Unbalanced brackets in " + opt_name + " '" + text + "'";
        return -1;
    }
    if (pairs.empty()) {
        err = opt_name + " must contain at least one key=value pair";
        return -1;
    }

    for (size_t i = 0; i < pairs.size(); ++i) {
        std::string p = pairs[i];
        size_t eq = p.find('=');
        if (eq == std::string::npos) {
            err = "Invalid " + opt_name + " pair '" + p + "'. Expected key=value";
            return -1;
        }
        std::string k = trim(p.substr(0, eq));
        std::string v = trim(p.substr(eq + 1));
        if (k.empty()) {
            err = "Invalid " + opt_name + " pair '" + p + "'. Key cannot be empty";
            return -1;
        }
        if (!v.empty() && v[0] == '[') v.erase(std::remove_if(v.begin(), v.end(), ::isspace), v.end());
        if (out.count(k)) {
            err = "Duplicate key '" + k + "' in " + opt_name;
            return -1;
        }
        out[k] = v;
    }
    return 0;
}

static int parse_attr_filter(const std::string& text, std::map<std::string, std::string>& out, std::string& err)
{
    return parse_key_value_filter("--attr", text, out, err);
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
    return parse_key_value_filter("--nattr", text, out, err);
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
      << "                        [--attr ATTR] [--attr-strict] [--attrkey ATTRKEY] [--nattrkey NATTRKEY]\n"
      << "                        [--nattr NATTR] [--regex-case-sensitive]\n"
      << "                        [--is_mtg2_switch {0,1,2}] [--is_legacy {0,1}] [--edition {1,2}]\n"
      << "                        [--show-encoding] [--show-sources]\n"
      << "                        [--format {line,table,json}] [--order {asc,desc}] [--order-by FIELD] [--no-truncate]\n";
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
      << "                        [--attr ATTR] [--attr-strict] [--attrkey ATTRKEY] [--nattrkey NATTRKEY]\n"
      << "                        [--nattr NATTR] [--regex-case-sensitive]\n"
      << "                        [--is_mtg2_switch {0,1,2}] [--is_legacy {0,1}] [--edition {1,2}]\n"
      << "                        [--show-encoding] [--show-sources]\n"
      << "                        [--format {line,table,json}] [--order {asc,desc}] [--order-by FIELD] [--no-truncate]\n"
      << "\n"
      << "Query parameter definitions in ecCodes concept files.\n"
      << "\n"
      << "options:\n"
      << "  -h, --help            show this help message and exit\n"
      << "  --definitions-root DEFINITIONS_ROOT\n"
      << "                        Use only these definitions directories, separated\n"
      << "                        by the path separator. Unlike ECCODES_DEFINITION_PATH,\n"
      << "                        the built-in definitions are not added and\n"
      << "                        ECCODES_EXTRA_DEFINITION_PATH is ignored, so another\n"
      << "                        definitions set, e.g. of an older ecCodes version,\n"
      << "                        can be queried on its own. A file present in several\n"
      << "                        directories is taken from the first that has it.\n"
      << "                        If omitted, the same path as ecCodes is used:\n"
      << "                        ECCODES_EXTRA_DEFINITION_PATH, then\n"
      << "                        ECCODES_DEFINITION_PATH or the built-in default (which\n"
      << "                        may be MEMFS), with the built-in default appended\n"
      << "  --debug-path-resolution\n"
      << "                        Print the ecCodes definitions path and its\n"
      << "                        directories to stderr. Also reports concept files\n"
      << "                        read from disk which a MEMFS build would not find,\n"
      << "                        and how many concept files were parsed and skipped\n"
      << "  --columns COLUMNS     Comma-separated list of output columns to show. Valid\n"
      << "                        columns: edition,is_mtg2_switch,is_legacy,scope,paramId,shortName,\n"
      << "                        name,units,encoding,sources\n"
      << "  --scope SCOPE         Comma-separated list of exact scopes to include,\n"
      << "                        e.g. 'grib2,grib2/localConcepts/ecmf'\n"
      << "  --name NAME           Wildcard pattern for name, e.g. '*precipitation*'\n"
      << "  --name-regex NAME_REGEX\n"
      << "                        PCRE2-style regex for name\n"
      << "  --shortName SHORTNAME\n"
      << "                        Wildcard pattern for shortName, e.g. '10*'\n"
      << "  --shortName-regex SHORTNAME_REGEX\n"
      << "                        PCRE2-style regex for shortName\n"
      << "  --paramId PARAMID     Wildcard pattern for paramId\n"
      << "  --paramId-regex PARAMID_REGEX\n"
      << "                        PCRE2-style regex for paramId\n"
      << "  --units UNITS         Wildcard pattern for units\n"
      << "  --units-regex UNITS_REGEX\n"
      << "                        PCRE2-style regex for units\n"
      << "  --attr ATTR           Filter by attribute key/value pairs as"
      << " \n"
      << "                        key=value,key=value. Non-strict mode matches entries"
      << " \n"
      << "                        containing at least these pairs. Array values as\n"
      << "                        printed by --show-encoding, e.g.\n"
      << "                        typeOfStatisticalProcessing=[102,2]\n"
      << "  --attr-strict         With --attr and/or --attrkey, require an exact\n"
      << "                        match (same keys and values)\n"
      << "  --attrkey ATTRKEY     Filter by attribute key presence (comma-separated).\n"
      << "                        Keeps only parameters that have all specified keys\n"
      << "  --nattrkey NATTRKEY   Exclude parameters with specific attribute keys\n"
      << "                        (comma-separated list)\n"
      << "  --nattr NATTR         Exclude parameters with specific attribute key/value\n"
      << "                        pairs as key=value,key=value\n"
      << "  --regex-case-sensitive\n"
      << "                        Make regex searches case-sensitive. By default regex\n"
      << "                        searches are case-insensitive\n"
      << "  --is_mtg2_switch {0,1,2}\n"
      << "                        Restrict GRIB2 to lte33=0, def=1, chemsplit=2\n"
      << "  --is_legacy {0,1}     Restrict GRIB2 to entries from *.legacy.def files (1)\n"
      << "                        or from all other files (0). ecCodes uses legacy\n"
      << "                        entries only as the last fallback, for every MTG2\n"
      << "                        switch value\n"
      << "  --edition {1,2}       Restrict to GRIB edition 1 or 2\n"
      << "  --show-encoding       Show the GRIB encoding constraints (same key/value\n"
      << "                        conditions)\n"
      << "  --show-sources        Show source definition files contributing to each\n"
      << "                        result\n"
      << "  --format {line,table,json}\n"
      << "                        Output format: line (default), table or json.\n"
      << "                        json prints an array with one object per result;\n"
      << "                        merged duplicates are arrays, missing values null\n"
      << "  --order {asc,desc}    Sort order for results by edition/switch/scope/paramId\n"
      << "                        ascending (default) or descending\n"
      << "  --order-by FIELD      Primary sort fields (comma-separated, in priority\n"
      << "                        order). Valid: edition,is_mtg2_switch,is_legacy,scope,\n"
      << "                        paramId,shortName,name,units\n"
      << "  --no-truncate         With --format table, show cells in full. By default\n"
      << "                        long cells are cut to a fixed maximum width per\n"
      << "                        column (e.g. name 48, encoding 60 characters)\n";
    return o.str();
}

static int parse_args(int argc, char** argv, Options& opt, std::string& err)
{
    if (argc == 1) return 0;  // no arguments - show short usage in main

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-h" || a == "--help") {
            return 1;  // explicit help request
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
        else if (key == "--is_mtg2_switch") {
            if (!need_value("--is_mtg2_switch")) return -1;
            // Check the whole value: atoi would turn "garbage" into 0
            if (val != "0" && val != "1" && val != "2") {
                err = "--is_mtg2_switch must be one of 0,1,2";
                return -1;
            }
            opt.has_is_mtg2_switch = true;
            opt.is_mtg2_switch = atoi(val.c_str());
        }
        else if (key == "--is_legacy") {
            if (!need_value("--is_legacy")) return -1;
            if (val != "0" && val != "1") {
                err = "--is_legacy must be one of 0,1";
                return -1;
            }
            opt.has_is_legacy = true;
            opt.is_legacy = atoi(val.c_str());
        }
        else if (key == "--edition") {
            if (!need_value("--edition")) return -1;
            // Check the whole value: atoi would turn "2junk" into 2
            if (val != "1" && val != "2") {
                err = "--edition must be 1 or 2";
                return -1;
            }
            opt.has_edition = true;
            opt.edition = atoi(val.c_str());
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
            if (!(opt.format == "line" || opt.format == "table" || opt.format == "json")) {
                err = "--format must be one of: line, table, json";
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
                "edition", "is_mtg2_switch", "is_legacy", "scope", "paramId", "shortName", "name", "units"
            };
            std::vector<std::string> fields = split(val, ',');
            if (fields.empty()) {
                err = "--order-by must contain at least one field";
                return -1;
            }
            for (size_t j = 0; j < fields.size(); ++j) {
                if (!valid_order_by.count(fields[j])) {
                    err = "--order-by must contain only: edition, is_mtg2_switch, is_legacy, scope, paramId, shortName, name, units";
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
    if (key == "is_legacy") return rec.edition == 1 ? "-" : (rec.legacy ? "1" : "0");
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

// JSON output (--format json)

static std::string json_escape(const std::string& s)
{
    std::string out = "\"";
    for (size_t i = 0; i < s.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                }
                else {
                    out += s[i];
                }
        }
    }
    return out + "\"";
}

static bool is_digit_at(const std::string& s, size_t i)
{
    return i < s.size() && isdigit(static_cast<unsigned char>(s[i]));
}

// True if s is a number in JSON syntax, e.g. 167, -1, 0.5 or 1e-05
static bool is_json_number(const std::string& s)
{
    size_t i = 0;
    if (i < s.size() && s[i] == '-') ++i;
    if (!is_digit_at(s, i)) return false;
    if (s[i] == '0') {
        ++i;
    }
    else {
        while (is_digit_at(s, i))
            ++i;
    }
    if (i < s.size() && s[i] == '.') {
        ++i;
        if (!is_digit_at(s, i)) return false;
        while (is_digit_at(s, i))
            ++i;
    }
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        ++i;
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
        if (!is_digit_at(s, i)) return false;
        while (is_digit_at(s, i))
            ++i;
    }
    return i == s.size();
}

static std::string json_scalar(const std::string& s, bool numeric)
{
    return (numeric && is_json_number(s)) ? s : json_escape(s);
}

// Encoding value as printed by --show-encoding: numbers stay numbers and
// integer arrays such as [102,2] become JSON arrays
static std::string json_attr_value(const std::string& v)
{
    if (v.size() >= 2 && v[0] == '[' && v[v.size() - 1] == ']') {
        const std::string inner = v.substr(1, v.size() - 2);
        std::string out = "[";
        bool all_numbers = true;
        size_t start = 0;
        while (!inner.empty()) {
            size_t comma = inner.find(',', start);
            const std::string item = trim(inner.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
            if (!is_json_number(item)) {
                all_numbers = false;
                break;
            }
            if (start) out += ", ";
            out += item;
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
        if (all_numbers) return out + "]";
    }
    return json_scalar(v, true);
}

// Concept values: one value as a scalar, merged duplicates as an array
static std::string json_values(const Record& rec, const std::string& key, bool numeric)
{
    std::map<std::string, std::vector<std::string> >::const_iterator it = rec.values.find(key);
    if (it == rec.values.end() || it->second.empty()) return "null";
    if (it->second.size() == 1) return json_scalar(it->second[0], numeric);
    std::string out = "[";
    for (size_t i = 0; i < it->second.size(); ++i) {
        if (i) out += ", ";
        out += json_scalar(it->second[i], numeric);
    }
    return out + "]";
}

static std::string json_value(const Record& rec, const std::string& key)
{
    if (key == "edition") return std::to_string(rec.edition);
    if (key == "is_mtg2_switch") return rec.sw < 0 ? "null" : std::to_string(rec.sw);
    if (key == "is_legacy") return rec.edition == 1 ? "null" : (rec.legacy ? "1" : "0");
    if (key == "scope") return json_escape(rec.scope);
    if (key == "paramId") return json_values(rec, key, true);
    if (key == "shortName" || key == "name" || key == "units") return json_values(rec, key, false);
    if (key == "encoding") {
        std::string out = "{";
        for (std::map<std::string, std::string>::const_iterator it = rec.attrs.begin(); it != rec.attrs.end(); ++it) {
            if (out.size() > 1) out += ", ";
            out += json_escape(it->first) + ": " + json_attr_value(it->second);
        }
        return out + "}";
    }
    if (key == "sources") {
        std::string out = "[";
        for (size_t i = 0; i < rec.sources.size(); ++i) {
            if (i) out += ", ";
            out += json_escape(rec.sources[i]);
        }
        return out + "]";
    }
    return "null";
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
            "edition", "is_mtg2_switch", "is_legacy", "scope", "paramId", "shortName", "name", "units", "encoding", "sources"
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

    // The definitions are found as in ecCodes, by ecCodes itself: --definitions-root replaces
    // the definitions path of the context (without the built-in default and
    // ECCODES_EXTRA_DEFINITION_PATH), before any definition file is looked up
    grib_context* ctx = grib_context_get_default();
    if (!opt.definitions_root.empty()) grib_context_set_definitions_path(ctx, opt.definitions_root.c_str());
    const char* def_path = grib_definition_path(ctx);
    const std::vector<std::string> dirs = definitions_dirs(ctx);

    bool used_filesystem_backend = false;
    bool used_memfs_backend = false;
    for (size_t i = 0; i < dirs.size(); ++i) {
        if (starts_with(dirs[i], "/MEMFS")) {
            used_memfs_backend = true;
        }
        else if (is_dir(dirs[i])) {
            used_filesystem_backend = true;
        }
        else if (!opt.definitions_root.empty()) {
            warningf("Ignoring non-directory definitions root: %s", dirs[i].c_str());
        }
    }

    if (opt.debug_path_resolution) {
        fprintf(stderr, "DEBUG: ecCodes definitions path: %s\n", def_path ? def_path : "<unavailable>");
        fprintf(stderr, "DEBUG: definitions directories (first match wins):\n");
        if (dirs.empty()) fprintf(stderr, "DEBUG:   <none>\n");
        for (size_t i = 0; i < dirs.size(); ++i)
            fprintf(stderr, "DEBUG:   %s\n", dirs[i].c_str());
    }

    // The concept files of all directories, in path order
    std::set<std::string> rel_files;
    bool scan_failed = false;
    for (size_t i = 0; i < dirs.size(); ++i) {
        if (list_concept_files(dirs[i], rel_files, opt.debug_path_resolution) != GRIB_SUCCESS) scan_failed = true;
    }

    FileFilter file_filter;
    file_filter.has_edition = opt.has_edition;
    file_filter.edition = opt.edition;
    file_filter.has_is_mtg2_switch = opt.has_is_mtg2_switch;
    file_filter.is_mtg2_switch = opt.is_mtg2_switch;
    file_filter.has_is_legacy = opt.has_is_legacy;
    file_filter.is_legacy = opt.is_legacy;
    if (!opt.scope.empty()) {
        std::vector<std::string> scope_items = split(opt.scope, ',');
        file_filter.scopes.insert(scope_items.begin(), scope_items.end());
    }

    std::map<std::string, Record> records_by_key;

    // Files on disk and in MEMFS are both read in-process: codes_fopen serves /MEMFS paths from memory
    auto parse_all = [&](const FileFilter* filter, bool debug) {
        size_t n_parsed = 0, n_skipped = 0;
        if (parse_concept_files(ctx, rel_files, records_by_key, filter, n_parsed, n_skipped) != GRIB_SUCCESS)
            scan_failed = true;
        if (debug) fprintf(stderr, "DEBUG: concept files parsed: %zu, skipped by filters: %zu\n", n_parsed, n_skipped);
    };

    // Whether the checks below which look at all definitions (any results at all, known --attr
    // keys and known scopes, with suggestions) could fail only because files were skipped
    auto needs_all_files = [&]() -> bool {
        if (records_by_key.empty()) return true;
        std::set<std::string> known_keys, known_scopes;
        for (std::map<std::string, Record>::const_iterator it = records_by_key.begin(); it != records_by_key.end(); ++it) {
            known_scopes.insert(it->second.scope);
            for (std::map<std::string, std::string>::const_iterator a = it->second.attrs.begin(); a != it->second.attrs.end(); ++a)
                known_keys.insert(a->first);
        }
        for (std::map<std::string, std::string>::const_iterator a = attr_filter.begin(); a != attr_filter.end(); ++a)
            if (!known_keys.count(a->first)) return true;
        for (std::set<std::string>::const_iterator s = file_filter.scopes.begin(); s != file_filter.scopes.end(); ++s)
            if (!known_scopes.count(*s)) return true;
        return false;
    };

    // Parsed also after a directory listing error, so that every failure is reported
    if (file_filter.active()) {
        parse_all(&file_filter, opt.debug_path_resolution);
        if (!scan_failed && needs_all_files()) {
            if (opt.debug_path_resolution) fprintf(stderr, "DEBUG: reading all concept files to check the query\n");
            records_by_key.clear();
            parse_all(NULL, false);
        }
    }
    else {
        parse_all(NULL, opt.debug_path_resolution);
    }
    release_previous_concept_list();
    // ecCodes fails on a broken concept file as well: no partial results
    if (scan_failed) {
        errorf("Some definition directories or concept files could not be read, see above");
        return 2;
    }

    std::string effective_backend = "none";
    if (used_memfs_backend && used_filesystem_backend)
        effective_backend = "mixed";
    else if (used_memfs_backend)
        effective_backend = "memfs";
    else if (used_filesystem_backend)
        effective_backend = "filesystem";

    if (opt.debug_path_resolution) {
        fprintf(stderr, "DEBUG: effective backend: %s\n", effective_backend.c_str());
    }

    if (records_by_key.empty()) {
        errorf("No valid definitions roots were found. Provide --definitions-root or set ECCODES_DEFINITION_PATH/ECCODES_EXTRA_DEFINITION_PATH.");
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
        for (size_t i = 0; i < scope_items.size(); ++i)
            scope_filter.insert(scope_items[i]);

        std::set<std::string> known_scopes;
        for (size_t i = 0; i < records.size(); ++i)
            known_scopes.insert(records[i].scope);

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
        if (opt.has_is_mtg2_switch) {
            if (rec.sw < 0 || rec.sw != opt.is_mtg2_switch) continue;
        }
        if (opt.has_is_legacy) {
            if (rec.edition == 1 || (rec.legacy ? 1 : 0) != opt.is_legacy) continue;
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
        c = cmp_ll(parse_id(first_value(a, "paramId")), parse_id(first_value(b, "paramId")));
        if (c) return c;
        return cmp_ll(a.legacy, b.legacy);  // as in ecCodes, legacy entries come last
    };

    auto cmp_records = [&](const Record& a, const Record& b) -> int {
        int c = 0;
        for (size_t i = 0; i < opt.order_by_fields.size(); ++i) {
            const std::string& field = opt.order_by_fields[i];
            if (field == "edition")
                c = cmp_ll(a.edition, b.edition);
            else if (field == "is_mtg2_switch")
                c = cmp_ll(a.sw, b.sw);
            else if (field == "is_legacy")
                c = cmp_ll(a.legacy, b.legacy);
            else if (field == "scope")
                c = cmp_str(a.scope, b.scope);
            else if (field == "paramId")
                c = cmp_ll(parse_id(first_value(a, "paramId")), parse_id(first_value(b, "paramId")));
            else if (field == "shortName")
                c = cmp_str(first_value(a, "shortName"), first_value(b, "shortName"));
            else if (field == "name")
                c = cmp_str(first_value(a, "name"), first_value(b, "name"));
            else if (field == "units")
                c = cmp_str(first_value(a, "units"), first_value(b, "units"));
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
            std::string key = std::to_string(r.edition) + "|" + std::to_string(r.sw) + "|" + std::to_string(r.legacy) + "|" + r.scope + "|";
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

        std::stable_sort(filtered.begin(), filtered.end(), record_before);
    }

    if (filtered.empty()) {
        if (opt.format == "json") {
            // Keep stdout valid JSON
            printf("[]\n");
            fprintf(stderr, "No matching parameters found.\n");
        }
        else {
            printf("No matching parameters found.\n");
        }
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
            if (r.legacy) line += "is_legacy=1 ";
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

    if (opt.format == "json") {
        std::vector<std::string> cols = selected_columns;
        if (cols.empty()) {
            cols.push_back("edition");
            cols.push_back("is_mtg2_switch");
            cols.push_back("is_legacy");
            cols.push_back("scope");
            cols.push_back("paramId");
            cols.push_back("shortName");
            cols.push_back("name");
            cols.push_back("units");
            if (opt.show_encoding) cols.push_back("encoding");
            if (opt.show_sources) cols.push_back("sources");
        }
        printf("[\n");
        for (size_t i = 0; i < filtered.size(); ++i) {
            std::string line = "  {";
            for (size_t c = 0; c < cols.size(); ++c) {
                if (c) line += ", ";
                line += json_escape(cols[c]) + ": " + json_value(filtered[i], cols[c]);
            }
            line += (i + 1 < filtered.size()) ? "}," : "}";
            printf("%s\n", line.c_str());
        }
        printf("]\n");
        return 0;
    }

    // table format
    std::vector<std::string> cols;
    if (!selected_columns.empty()) {
        cols = selected_columns;
    }
    else {
        bool include_switch = false, include_legacy = false;
        for (size_t i = 0; i < filtered.size(); ++i) {
            if (filtered[i].sw >= 0) include_switch = true;
            if (filtered[i].legacy) include_legacy = true;
        }
        cols.push_back("edition");
        if (include_switch) cols.push_back("is_mtg2_switch");
        if (include_legacy) cols.push_back("is_legacy");
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
    for (size_t c = 0; c < cols.size(); ++c)
        widths[c] = cols[c].size();
    for (size_t r = 0; r < rows.size(); ++r) {
        for (size_t c = 0; c < cols.size(); ++c) {
            widths[c] = std::max(widths[c], rows[r][c].size());
        }
    }

    if (!opt.no_truncate) {
        std::map<std::string, size_t> maxw;
        maxw["edition"] = 7;
        maxw["is_mtg2_switch"] = 14;
        maxw["is_legacy"] = 9;
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
