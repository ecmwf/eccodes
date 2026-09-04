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
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <vector>

#include "accessor/Accessor.h"

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>

static grib_handle* s_completion_handle = NULL;
static std::vector<std::string> s_completion_candidates;
static size_t s_completion_index = 0;
static std::set<std::string> s_session_symbols;

static bool starts_with(const std::string& value, const std::string& prefix)
{
    return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

static std::set<std::string> collect_accessor_names()
{
    std::set<std::string> names;
    const std::vector<std::string> registered = eccodes::AccessorFactory::instance().types();
    names.insert(registered.begin(), registered.end());
    return names;
}

static void print_accessors(const std::string& pattern = std::string(), bool ignore_case = false)
{
    const std::set<std::string> accessors = collect_accessor_names();
    if (pattern.empty()) {
        printf("Accessors (%zu):\n", accessors.size());
        for (const auto& name : accessors) {
            printf("  %s\n", name.c_str());
        }
        return;
    }

    std::regex re;
    try {
        re = ignore_case ? std::regex(pattern, std::regex_constants::icase) : std::regex(pattern);
    }
    catch (const std::regex_error& e) {
        fprintf(stderr, "codes_interpreter: invalid regex '%s' (%s)\n", pattern.c_str(), e.what());
        return;
    }

    size_t matched = 0;
    for (const auto& name : accessors) {
        if (std::regex_search(name, re)) {
            ++matched;
        }
    }

    printf("Accessors matching /%s/ (%zu):\n", pattern.c_str(), matched);
    for (const auto& name : accessors) {
        if (std::regex_search(name, re)) {
            printf("  %s\n", name.c_str());
        }
    }
}

static std::set<std::string> collect_key_names(grib_handle* h)
{
    std::set<std::string> keys;
    if (!h) {
        return keys;
    }

    grib_keys_iterator* it = grib_keys_iterator_new(h, GRIB_KEYS_ITERATOR_SKIP_DUPLICATES, NULL);
    if (!it) {
        return keys;
    }

    while (grib_keys_iterator_next(it)) {
        const char* name = grib_keys_iterator_get_name(it);
        if (name && *name) {
            keys.insert(name);
        }
    }
    grib_keys_iterator_delete(it);
    return keys;
}

static std::set<std::string> collect_functor_names()
{
    // Mirrors callable expression functors implemented in expression/Functor.cc
    static const char* functors[] = {
        "new", "defined", "changed", "missing", "max", "min", "abs", "size",
        "element", "debug_mode", "dump_content", "environment_variable", "contains",
        "is_one_of", "gribex_mode_on"
    };

    std::set<std::string> names;
    for (const char* f : functors) {
        names.insert(f);
    }
    return names;
}

static std::vector<std::string> extract_declared_symbols(const std::string& statement)
{
    std::vector<std::string> symbols;
    std::string text = statement;

    auto is_word_char = [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    };

    for (size_t i = 0; i < text.size();) {
        if (!std::isalpha(static_cast<unsigned char>(text[i]))) {
            ++i;
            continue;
        }

        size_t wstart = i;
        while (i < text.size() && is_word_char(text[i])) {
            ++i;
        }
        std::string word = text.substr(wstart, i - wstart);
        if (word != "meta" && word != "transient") {
            continue;
        }

        while (i < text.size() && (text[i] == ' ' || text[i] == '\t' || text[i] == '\n' || text[i] == '\r')) {
            ++i;
        }
        if (i >= text.size() || !is_word_char(text[i])) {
            continue;
        }

        size_t nstart = i;
        while (i < text.size() && is_word_char(text[i])) {
            ++i;
        }
        symbols.push_back(text.substr(nstart, i - nstart));
    }

    std::sort(symbols.begin(), symbols.end());
    symbols.erase(std::unique(symbols.begin(), symbols.end()), symbols.end());
    return symbols;
}

static char* completion_generator(const char* text, int state)
{
    if (state == 0) {
        s_completion_candidates.clear();
        s_completion_index = 0;

        const std::string prefix = text ? text : "";
        const bool bracketed_key = !prefix.empty() && prefix[0] == '[';
        const std::string key_prefix = bracketed_key ? prefix.substr(1) : prefix;

        static const char* kWords[] = {
            "print", "set", "meta", "transient", "if", "else", "while", "switch",
            "assert", "write", "remove", "rename", "concept", "alias", "quit", "exit",
            "help", "info", "changes", "list", "next", "prev", "goto", "save", "load", "undo", "diff", "--values", "--ignore-case", "-i",
            ":help", ":info", ":changes", ":list", ":next", ":prev", ":goto", ":save", ":load", ":undo", ":diff"
        };

        std::set<std::string> all_candidates;
        if (!bracketed_key) {
            for (const char* word : kWords) {
                all_candidates.insert(word);
            }

            const std::set<std::string> accessors = collect_accessor_names();
            all_candidates.insert(accessors.begin(), accessors.end());

            const std::set<std::string> functors = collect_functor_names();
            all_candidates.insert(functors.begin(), functors.end());
        }

        const std::set<std::string> keys = collect_key_names(s_completion_handle);
        for (const auto& key : keys) {
            if (starts_with(key, key_prefix)) {
                all_candidates.insert(bracketed_key ? ("[" + key + "]") : key);
            }
        }

        for (const auto& symbol : s_session_symbols) {
            if (starts_with(symbol, key_prefix)) {
                all_candidates.insert(bracketed_key ? ("[" + symbol + "]") : symbol);
            }
        }

        for (const auto& candidate : all_candidates) {
            if (starts_with(candidate, prefix)) {
                s_completion_candidates.push_back(candidate);
            }
        }
    }

    if (s_completion_index >= s_completion_candidates.size()) {
        return NULL;
    }

    return strdup(s_completion_candidates[s_completion_index++].c_str());
}

static char** codes_interpreter_completion(const char* text, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 0;
    return rl_completion_matches(text, completion_generator);
}
#endif

static std::string find_definitions_path(const char* argv0)
{
    std::vector<std::string> candidates;

    candidates.push_back("./definitions");

    if (argv0 && argv0[0] != '\0') {
        std::string exe(argv0);
        std::string::size_type slash = exe.find_last_of("/");
        std::string dir = (slash == std::string::npos) ? "." : exe.substr(0, slash);

        std::vector<std::string> local_dirs = {
            dir,
            dir + "/..",
            dir + "/../..",
            dir + "/../../..",
            dir + "/../../share/eccodes",
            dir + "/../share/eccodes",
            dir + "/../definitions",
            dir + "/../../definitions",
            dir + "/definitions",
            dir + "/share/eccodes/definitions"
        };

        for (const auto& d : local_dirs) {
            candidates.push_back(d + "/definitions");
            candidates.push_back(d + "/share/eccodes/definitions");
        }
    }

    candidates.push_back("/usr/local/share/eccodes/definitions");
    candidates.push_back("/usr/share/eccodes/definitions");

    for (const auto& p : candidates) {
        if (!p.empty() && access((p + "/boot.def").c_str(), R_OK) == 0) {
            return p;
        }
    }
    return std::string();
}

static bool is_complete_statement(const std::string& text)
{
    int brace_depth = 0;
    int paren_depth = 0;
    bool in_string = false;
    char quote = '\0';

    for (size_t i = 0; i < text.size(); ++i) {
        const char c = text[i];

        if (in_string) {
            if (c == '\\' && i + 1 < text.size()) {
                ++i;
                continue;
            }
            if (c == quote) {
                in_string = false;
                quote = '\0';
            }
            continue;
        }

        if (c == '"' || c == '\'') {
            in_string = true;
            quote = c;
            continue;
        }

        if (c == '{') {
            ++brace_depth;
            continue;
        }
        if (c == '}') {
            --brace_depth;
            continue;
        }
        if (c == '(') {
            ++paren_depth;
            continue;
        }
        if (c == ')') {
            --paren_depth;
            continue;
        }
    }

    std::string trimmed = text;
    while (!trimmed.empty() && (trimmed.back() == '\n' || trimmed.back() == '\r' || trimmed.back() == ' ' || trimmed.back() == '\t')) {
        trimmed.pop_back();
    }

    if (in_string || brace_depth != 0 || paren_depth != 0 || trimmed.empty()) {
        return false;
    }

    return trimmed.size() >= 1 && trimmed.back() == ';';
}

static std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\n' || s[start] == '\r')) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\n' || s[end - 1] == '\r')) {
        --end;
    }
    return s.substr(start, end - start);
}

static bool starts_with_keyword(const std::string& text, const char* keyword)
{
    const size_t keyword_len = strlen(keyword);
    if (text.size() < keyword_len) {
        return false;
    }
    if (text.compare(0, keyword_len, keyword) != 0) {
        return false;
    }
    if (text.size() == keyword_len) {
        return true;
    }

    const char next = text[keyword_len];
    return next == ' ' || next == '\t' || next == '(' || next == '{' || next == '"';
}

static void parse_command_flags(const std::string& args, bool* ignore_case, bool* touched, bool* values, std::string* pattern)
{
    if (ignore_case) {
        *ignore_case = false;
    }
    if (touched) {
        *touched = false;
    }
    if (values) {
        *values = false;
    }
    if (pattern) {
        pattern->clear();
    }

    size_t pos = 0;
    while (pos < args.size() && (args[pos] == ' ' || args[pos] == '\t')) {
        ++pos;
    }

    while (pos < args.size()) {
        size_t end = pos;
        while (end < args.size() && args[end] != ' ' && args[end] != '\t') {
            ++end;
        }

        const std::string token = args.substr(pos, end - pos);
        bool consumed = false;

        if (ignore_case && (token == "--ignore-case" || token == "-i")) {
            *ignore_case = true;
            consumed = true;
        }
        else if (touched && token == "--touched") {
            *touched = true;
            consumed = true;
        }
        else if (values && token == "--values") {
            *values = true;
            consumed = true;
        }

        if (!consumed) {
            if (pattern) {
                *pattern = trim(args.substr(pos));
            }
            return;
        }

        pos = end;
        while (pos < args.size() && (args[pos] == ' ' || args[pos] == '\t')) {
            ++pos;
        }
    }
}

static bool should_persist_statement(const std::string& statement)
{
    const std::string text = trim(statement);
    if (text.empty()) {
        return false;
    }

    if (starts_with_keyword(text, "print") ||
        starts_with_keyword(text, "write") ||
        starts_with_keyword(text, "assert") ||
        starts_with_keyword(text, "close")) {
        return false;
    }

    return true;
}

static bool parse_positive_long(const std::string& text, long* value)
{
    if (!value) {
        return false;
    }
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    errno = 0;
    long v = strtol(text.c_str(), &end, 10);
    if (errno != 0 || end == text.c_str() || *end != '\0' || v <= 0) {
        return false;
    }
    *value = v;
    return true;
}

struct KeyChange
{
    std::string name;
    std::string before;
    std::string after;
};

static bool get_scalar_key_value(grib_handle* h, const std::string& key, std::string& value)
{
    int type = GRIB_TYPE_UNDEFINED;
    int err = grib_get_native_type(h, key.c_str(), &type);
    if (err != GRIB_SUCCESS) {
        return false;
    }

    size_t count = 0;
    err = grib_get_size(h, key.c_str(), &count);
    if (err != GRIB_SUCCESS || count != 1) {
        return false;
    }

    char buffer[128] = {0,};
    if (type == GRIB_TYPE_LONG) {
        long lv = 0;
        if (grib_get_long(h, key.c_str(), &lv) != GRIB_SUCCESS) {
            return false;
        }
        snprintf(buffer, sizeof(buffer), "L:%ld", lv);
        value.assign(buffer);
        return true;
    }

    if (type == GRIB_TYPE_DOUBLE) {
        double dv = 0;
        if (grib_get_double(h, key.c_str(), &dv) != GRIB_SUCCESS) {
            return false;
        }
        snprintf(buffer, sizeof(buffer), "D:%.17g", dv);
        value.assign(buffer);
        return true;
    }

    if (type == GRIB_TYPE_STRING || type == GRIB_TYPE_LABEL) {
        size_t len = 128;
        std::vector<char> s(len + 1, '\0');
        err = grib_get_string(h, key.c_str(), s.data(), &len);
        if (err == GRIB_BUFFER_TOO_SMALL) {
            s.assign(len + 1, '\0');
            err = grib_get_string(h, key.c_str(), s.data(), &len);
        }
        if (err != GRIB_SUCCESS) {
            return false;
        }
        value.assign("S:");
        value.append(s.data());
        return true;
    }

    return false;
}

static std::vector<KeyChange> compute_changed_scalar_keys(grib_handle* before, grib_handle* after)
{
    std::vector<KeyChange> changed;
    std::set<std::string> all_keys = collect_key_names(before);
    const std::set<std::string> after_keys = collect_key_names(after);
    all_keys.insert(after_keys.begin(), after_keys.end());

    for (const auto& key : all_keys) {
        std::string before_value;
        std::string after_value;
        const bool has_before = get_scalar_key_value(before, key, before_value);
        const bool has_after = get_scalar_key_value(after, key, after_value);

        if (!has_before && !has_after) {
            continue;
        }
        if (has_before != has_after || before_value != after_value) {
            KeyChange e;
            e.name = key;
            e.before = has_before ? before_value : "<undefined>";
            e.after = has_after ? after_value : "<undefined>";
            changed.push_back(e);
        }
    }

    return changed;
}

static std::vector<KeyChange> compute_touched_unchanged_scalar_keys(grib_handle* before,
                                                                    grib_handle* after,
                                                                    const std::vector<KeyChange>& changed)
{
    std::vector<KeyChange> touched;
    std::set<std::string> changed_names;
    for (const auto& item : changed) {
        changed_names.insert(item.name);
    }

    std::set<std::string> touched_names = changed_names;
    bool added = true;
    while (added) {
        added = false;
        for (grib_dependency* dep = after ? after->dependencies : NULL; dep != NULL; dep = dep->next) {
            if (!dep->observed || !dep->observer || !dep->observed->name_ || !dep->observer->name_) {
                continue;
            }
            const std::string observed = dep->observed->name_;
            const std::string observer = dep->observer->name_;
            if (touched_names.find(observed) != touched_names.end()) {
                if (touched_names.insert(observer).second) {
                    added = true;
                }
            }
        }
    }

    for (const auto& key : touched_names) {
        if (changed_names.find(key) != changed_names.end()) {
            continue;
        }

        std::string before_value;
        std::string after_value;
        const bool has_before = get_scalar_key_value(before, key, before_value);
        const bool has_after  = get_scalar_key_value(after, key, after_value);
        if (!has_before || !has_after) {
            continue;
        }
        if (before_value != after_value) {
            continue;
        }

        KeyChange e;
        e.name   = key;
        e.before = before_value;
        e.after  = after_value;
        touched.push_back(e);
    }
    std::sort(touched.begin(), touched.end(), [](const KeyChange& a, const KeyChange& b) {
        return a.name < b.name;
    });
    return touched;
}

static void print_changed_keys(const std::vector<KeyChange>& keys)
{
    printf("Changed keys (%zu):\n", keys.size());
    for (const auto& key : keys) {
        printf("  %s: %s -> %s\n", key.name.c_str(), key.before.c_str(), key.after.c_str());
    }
}

static void write_changed_keys(std::ofstream& out, const std::vector<KeyChange>& keys)
{
    out << "Changed keys (" << keys.size() << "):\n";
    for (const auto& key : keys) {
        out << "  " << key.name << ": " << key.before << " -> " << key.after << "\n";
    }
}

static bool compile_regex_or_report(const std::string& pattern, std::regex& re, bool ignore_case = false)
{
    try {
        re = ignore_case ? std::regex(pattern, std::regex_constants::icase) : std::regex(pattern);
    }
    catch (const std::regex_error& e) {
        fprintf(stderr, "codes_interpreter: invalid regex '%s' (%s)\n", pattern.c_str(), e.what());
        return false;
    }
    return true;
}

static void print_changed_keys_filtered(const std::vector<KeyChange>& keys, const std::string& pattern, bool ignore_case = false)
{
    if (pattern.empty()) {
        print_changed_keys(keys);
        return;
    }

    std::regex re;
    if (!compile_regex_or_report(pattern, re, ignore_case)) {
        return;
    }

    std::vector<KeyChange> matched;
    for (const auto& key : keys) {
        if (std::regex_search(key.name, re)) {
            matched.push_back(key);
        }
    }

    printf("Changed keys matching /%s/ (%zu):\n", pattern.c_str(), matched.size());
    for (const auto& key : matched) {
        printf("  %s: %s -> %s\n", key.name.c_str(), key.before.c_str(), key.after.c_str());
    }
}

static void print_touched_keys(const std::vector<KeyChange>& keys)
{
    printf("Touched but unchanged keys (%zu):\n", keys.size());
    for (const auto& key : keys) {
        printf("  %s: %s -> %s\n", key.name.c_str(), key.before.c_str(), key.after.c_str());
    }
}

static void print_touched_keys_filtered(const std::vector<KeyChange>& keys, const std::string& pattern, bool ignore_case = false)
{
    if (pattern.empty()) {
        print_touched_keys(keys);
        return;
    }

    std::regex re;
    if (!compile_regex_or_report(pattern, re, ignore_case)) {
        return;
    }

    std::vector<KeyChange> matched;
    for (const auto& key : keys) {
        if (std::regex_search(key.name, re)) {
            matched.push_back(key);
        }
    }

    printf("Touched but unchanged keys matching /%s/ (%zu):\n", pattern.c_str(), matched.size());
    for (const auto& key : matched) {
        printf("  %s: %s -> %s\n", key.name.c_str(), key.before.c_str(), key.after.c_str());
    }
}

static std::string describe_key_value_for_list(grib_handle* h, const std::string& key)
{
    std::string value;
    if (get_scalar_key_value(h, key, value)) {
        return value;
    }

    size_t size = 0;
    if (grib_get_size(h, key.c_str(), &size) == GRIB_SUCCESS) {
        if (size > 1) {
            char buffer[64] = {0,};
            snprintf(buffer, sizeof(buffer), "<array:%zu>", size);
            return std::string(buffer);
        }
        return std::string("<unavailable>");
    }
    return std::string("<undef>");
}

static void print_keys(grib_handle* h, const std::string& pattern = std::string(), bool with_values = false, bool ignore_case = false)
{
    const std::set<std::string> keys = collect_key_names(h);
    if (pattern.empty()) {
        if (with_values) {
            printf("Keys with values (%zu):\n", keys.size());
        }
        else {
            printf("Keys (%zu):\n", keys.size());
        }
        for (const auto& key : keys) {
            if (with_values) {
                const std::string described = describe_key_value_for_list(h, key);
                printf("  %s = %s\n", key.c_str(), described.c_str());
            }
            else {
                printf("  %s\n", key.c_str());
            }
        }
        return;
    }

    std::regex re;
    if (!compile_regex_or_report(pattern, re, ignore_case)) {
        return;
    }

    size_t matched = 0;
    for (const auto& key : keys) {
        if (std::regex_search(key, re)) {
            ++matched;
        }
    }

    if (with_values) {
        printf("Keys with values matching /%s/ (%zu):\n", pattern.c_str(), matched);
    }
    else {
        printf("Keys matching /%s/ (%zu):\n", pattern.c_str(), matched);
    }
    for (const auto& key : keys) {
        if (std::regex_search(key, re)) {
            if (with_values) {
                const std::string described = describe_key_value_for_list(h, key);
                printf("  %s = %s\n", key.c_str(), described.c_str());
            }
            else {
                printf("  %s\n", key.c_str());
            }
        }
    }
}

static std::vector<KeyChange> merge_declared_symbol_changes(std::vector<KeyChange> base_changes,
                                                            const std::vector<std::string>& declared_symbols,
                                                            grib_handle* before,
                                                            grib_handle* after)
{
    std::unordered_map<std::string, size_t> pos;
    for (size_t i = 0; i < base_changes.size(); ++i) {
        pos[base_changes[i].name] = i;
    }

    for (const auto& name : declared_symbols) {
        if (pos.find(name) != pos.end()) {
            continue;
        }

        std::string before_value;
        std::string after_value;
        const bool has_before = get_scalar_key_value(before, name, before_value);
        const bool has_after = get_scalar_key_value(after, name, after_value);

        KeyChange e;
        e.name = name;
        e.before = has_before ? before_value : "<undefined>";
        e.after = has_after ? after_value : "<declared>";
        base_changes.push_back(e);
    }

    std::sort(base_changes.begin(), base_changes.end(), [](const KeyChange& a, const KeyChange& b) {
        return a.name < b.name;
    });
    return base_changes;
}

static std::string read_text_file(const std::string& path, bool* ok)
{
    if (ok)
        *ok = false;
    std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
    if (!in.good()) {
        return std::string();
    }

    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (ok)
        *ok = true;
    return content;
}

static bool write_text_file(const std::string& path, const std::string& content)
{
    std::ofstream out(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!out.good()) {
        return false;
    }
    out << content;
    return out.good();
}

static std::string build_session_script(const std::vector<std::string>& statements)
{
    std::string result;
    for (const auto& statement : statements) {
        if (statement.empty()) {
            continue;
        }
        result += statement;
        if (result.empty() || result.back() != '\n') {
            result += "\n";
        }
    }
    return result;
}

static std::vector<std::string> collect_declared_symbols_from_statements(const std::vector<std::string>& statements)
{
    std::vector<std::string> all;
    for (const auto& statement : statements) {
        const std::vector<std::string> declared = extract_declared_symbols(statement);
        all.insert(all.end(), declared.begin(), declared.end());
    }
    std::sort(all.begin(), all.end());
    all.erase(std::unique(all.begin(), all.end()), all.end());
    return all;
}

static std::vector<off_t> collect_message_offsets(const char* filename, int* err)
{
    std::vector<off_t> offsets;
    if (err) {
        *err = GRIB_SUCCESS;
    }

    FILE* input = fopen(filename, "rb");
    if (!input) {
        if (err) {
            *err = GRIB_IO_PROBLEM;
        }
        return offsets;
    }

    while (true) {
        const off_t pos = ftello(input);
        int local_err = 0;
        grib_handle* h = grib_handle_new_from_file(grib_context_get_default(), input, &local_err);
        if (!h) {
            if (local_err != GRIB_SUCCESS && local_err != GRIB_END_OF_FILE) {
                if (err) {
                    *err = local_err;
                }
            }
            break;
        }
        offsets.push_back(pos);
        grib_handle_delete(h);
    }

    fclose(input);
    return offsets;
}

static grib_handle* load_message_from_offset(const char* filename, off_t offset, int* err)
{
    if (err) {
        *err = GRIB_SUCCESS;
    }

    FILE* input = fopen(filename, "rb");
    if (!input) {
        if (err) {
            *err = GRIB_IO_PROBLEM;
        }
        return NULL;
    }

    if (fseeko(input, offset, SEEK_SET) != 0) {
        fclose(input);
        if (err) {
            *err = GRIB_IO_PROBLEM;
        }
        return NULL;
    }

    int local_err = 0;
    grib_handle* h = grib_handle_new_from_file(grib_context_get_default(), input, &local_err);
    fclose(input);

    if (!h) {
        if (err) {
            *err = local_err;
        }
        return NULL;
    }

    return h;
}

static int apply_script(grib_handle* h, const std::string& script)
{
    if (script.empty()) {
        return GRIB_SUCCESS;
    }

    int fd = -1;
    char name[] = "/tmp/codes_interpreter.XXXXXX";
    fd = mkstemp(name);
    if (fd < 0) {
        fprintf(stderr, "codes_interpreter: cannot create temporary script file\n");
        return GRIB_IO_PROBLEM;
    }

    FILE* f = fdopen(fd, "w");
    if (!f) {
        fprintf(stderr, "codes_interpreter: cannot open temporary script file\n");
        close(fd);
        unlink(name);
        return GRIB_IO_PROBLEM;
    }

    fprintf(f, "%s", script.c_str());
    fclose(f);

    grib_action* a = grib_action_from_filter(name);
    if (!a) {
        fprintf(stderr, "codes_interpreter: unable to parse script\n");
        unlink(name);
        return GRIB_INVALID_ARGUMENT;
    }

    int err = grib_handle_apply_action(h, a);
    delete a;
    unlink(name);

    return err;
}

static grib_handle* replay_session(const grib_handle* base_handle, long current_message, long total_messages,
                                   const std::string& session_script, const std::string& statement, int* err)
{
    grib_handle* trial_handle = grib_handle_clone(base_handle);
    if (!trial_handle) {
        if (err)
            *err = GRIB_OUT_OF_MEMORY;
        return NULL;
    }

    // grib_handle_clone resets context counters internally; restore stream counters
    grib_context_set_handle_file_count(trial_handle->context, static_cast<int>(current_message));
    grib_context_set_handle_total_count(trial_handle->context, static_cast<int>(total_messages));

    std::string combined_script = session_script;
    if (!statement.empty()) {
        combined_script += statement;
        combined_script += "\n";
    }

    if (!combined_script.empty()) {
        int apply_err = apply_script(trial_handle, combined_script);
        if (apply_err != GRIB_SUCCESS) {
            if (err)
                *err = apply_err;
            grib_handle_delete(trial_handle);
            return NULL;
        }
    }

    if (err)
        *err = GRIB_SUCCESS;
    return trial_handle;
}

static void print_usage(const char* program)
{
    fprintf(stderr, "Usage: %s [--non-fail|-n] [--log-key-changes] [--log-session FILE] [--message|-m N] [--help|-h] <message_file>\n", program);
    fprintf(stderr, "Open one GRIB/BUFR/GTS message and evaluate ecCodes filter statements from standard input.\n");
    fprintf(stderr, "  --non-fail, -n  Keep the interpreter open after a statement fails\n");
    fprintf(stderr, "  --log-key-changes Enable key-diff tracking for :changes (off by default)\n");
    fprintf(stderr, "  --log-session FILE Append session input/output trace to FILE\n");
    fprintf(stderr, "  --message, -m N Open message number N (1-based) from file\n");
    fprintf(stderr, "  --help, -h      Show this help message\n");
}

int main(int argc, char* argv[])
{
    bool non_fail = false;
    bool log_key_changes = false;
    long selected_message = 1;
    std::string log_session_file;
    int file_arg = -1;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--non-fail" || arg == "-n") {
            non_fail = true;
            continue;
        }
        if (arg == "--log-key-changes") {
            log_key_changes = true;
            continue;
        }
        if (arg == "--log-session") {
            if (i + 1 >= argc) {
                print_usage(argv[0]);
                return 2;
            }
            log_session_file = argv[++i];
            continue;
        }
        if (arg == "--message" || arg == "-m") {
            if (i + 1 >= argc || !parse_positive_long(argv[i + 1], &selected_message)) {
                print_usage(argv[0]);
                return 2;
            }
            ++i;
            continue;
        }
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
        if (file_arg == -1) {
            file_arg = i;
            continue;
        }
        print_usage(argv[0]);
        return 2;
    }

    if (file_arg == -1) {
        print_usage(argv[0]);
        return 2;
    }

    std::ofstream session_log;
    if (!log_session_file.empty()) {
        session_log.open(log_session_file.c_str(), std::ios::app);
        if (!session_log.good()) {
            fprintf(stderr, "codes_interpreter: cannot open session log file '%s'\n", log_session_file.c_str());
            return 1;
        }
    }

    const char* env_defs = getenv("ECCODES_DEFINITION_PATH");
    const char* env_legacy_defs = getenv("GRIB_DEFINITION_PATH");
#ifndef HAVE_MEMFS
    if ((!env_defs || env_defs[0] == '\0') && (!env_legacy_defs || env_legacy_defs[0] == '\0')) {
        std::string defs = find_definitions_path(argv[0]);
        if (!defs.empty()) {
            setenv("ECCODES_DEFINITION_PATH", defs.c_str(), 1);
            grib_context_set_definitions_path(grib_context_get_default(), defs.c_str());
        }
    }
#endif
    int err = 0;
    std::vector<off_t> message_offsets = collect_message_offsets(argv[file_arg], &err);
    if (message_offsets.empty()) {
        fprintf(stderr, "codes_interpreter: cannot decode any message from '%s'", argv[file_arg]);
        if (err != GRIB_SUCCESS) {
            fprintf(stderr, ": %s", grib_get_error_message(err));
        }
        fprintf(stderr, "\n");
        return 1;
    }
    if (selected_message > static_cast<long>(message_offsets.size())) {
        fprintf(stderr, "codes_interpreter: message index %ld out of range (1..%zu)\n", selected_message, message_offsets.size());
        return 1;
    }

    long current_message = selected_message;
    grib_handle* h = load_message_from_offset(argv[file_arg], message_offsets[current_message - 1], &err);
    if (!h) {
        fprintf(stderr, "codes_interpreter: cannot decode message %ld from '%s': %s\n", current_message, argv[file_arg], grib_get_error_message(err));
        return 1;
    }

    grib_handle* base_handle = grib_handle_clone(h);
    if (!base_handle) {
        fprintf(stderr, "codes_interpreter: cannot clone message '%s'\n", argv[file_arg]);
        grib_handle_delete(h);
        return GRIB_OUT_OF_MEMORY;
    }

    grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
    grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
    grib_context_set_handle_file_count(base_handle->context, static_cast<int>(current_message));
    grib_context_set_handle_total_count(base_handle->context, static_cast<int>(message_offsets.size()));

    std::string script;
    std::string session_script;
    std::vector<std::string> session_statements;
    std::vector<KeyChange> last_changed_keys;
    std::vector<KeyChange> last_touched_unchanged_keys;
    char line[4096];

    printf("\n=== codes_interpreter started ===\n");
    printf("ecCodes version %d.%d.%d\n\n", ECCODES_MAJOR_VERSION, ECCODES_MINOR_VERSION, ECCODES_REVISION_VERSION);
    printf("Message: %s\n", argv[file_arg]);
    printf("Selected message: %ld/%zu\n", current_message, message_offsets.size());
    printf("Type a filter expression and end with ';' or type 'quit' to exit.\n");
    printf("Navigation: :next, :prev, :goto N, :info, :list, :accessors, :changes, :diff, :save, :load, :undo, :help\n");

#ifdef HAVE_LIBREADLINE
    using_history();
    rl_attempted_completion_function = codes_interpreter_completion;
    s_completion_handle = h;
#endif

    while (true) {
#ifdef HAVE_LIBREADLINE
        char* input = readline("codes_interpreter> ");
        if (!input) {
            printf("\n");
            break;
        }
        std::string text(input);
        free(input);
#else
        printf("codes_interpreter> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        std::string text(line);
#endif

        if (session_log.is_open()) {
            std::string logged = text;
            while (!logged.empty() && (logged.back() == '\n' || logged.back() == '\r')) {
                logged.pop_back();
            }
            session_log << "codes_interpreter> " << logged << "\n";
            session_log.flush();
        }

        std::string command = trim(text);
        if (command == "quit" || command == "exit") {
            break;
        }

        bool handled_navigation = false;
        auto clear_session_state = [&]() {
            script.clear();
            session_script.clear();
            session_statements.clear();
            last_changed_keys.clear();
            last_touched_unchanged_keys.clear();
#ifdef HAVE_LIBREADLINE
            s_session_symbols.clear();
#endif
        };

        auto try_navigation = [&](long next_message) -> bool {
            if (next_message < 1 || next_message > static_cast<long>(message_offsets.size())) {
                fprintf(stderr, "codes_interpreter: message index %ld out of range (1..%zu)\n", next_message, message_offsets.size());
                return true;
            }

            grib_handle* loaded = load_message_from_offset(argv[file_arg], message_offsets[next_message - 1], &err);
            if (!loaded) {
                fprintf(stderr, "codes_interpreter: cannot decode message %ld from '%s': %s\n", next_message, argv[file_arg], grib_get_error_message(err));
                return true;
            }

            grib_handle* loaded_base = grib_handle_clone(loaded);
            if (!loaded_base) {
                fprintf(stderr, "codes_interpreter: cannot clone message %ld\n", next_message);
                grib_handle_delete(loaded);
                return true;
            }

            grib_handle_delete(h);
            grib_handle_delete(base_handle);
            h = loaded;
            base_handle = loaded_base;
            current_message = next_message;
            grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
            grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
            grib_context_set_handle_file_count(base_handle->context, static_cast<int>(current_message));
            grib_context_set_handle_total_count(base_handle->context, static_cast<int>(message_offsets.size()));
            clear_session_state();
#ifdef HAVE_LIBREADLINE
            s_completion_handle = h;
#endif
            printf("Switched to message %ld/%zu\n", current_message, message_offsets.size());
            return true;
        };

        if (script.empty()) {
            if (command == ":help" || command == "help") {
                printf("Commands: quit, exit, :next, :prev, :goto N, :info, :list [--values] [--ignore-case|-i] [regex], :accessors [--ignore-case|-i] [regex], :changes [--touched] [--ignore-case|-i] [regex], :diff [--ignore-case|-i] [regex], :save FILE, :load FILE, :undo, :help\n");
                printf("Switching message resets session state (meta/transient/set history).\n");
                handled_navigation = true;
            }
            else if (command == ":info" || command == "info") {
                printf("Message %ld/%zu from %s\n", current_message, message_offsets.size(), argv[file_arg]);
                handled_navigation = true;
            }
            else if (command == ":changes" || command == "changes" || starts_with(command, ":changes ") || starts_with(command, "changes ")) {
                if (!log_key_changes) {
                    printf("not activated - use --log-key-changes\n");
                }
                else {
                    const bool has_args = starts_with(command, ":changes ") || starts_with(command, "changes ");
                    const size_t offset = command[0] == ':' ? 9 : 8;
                    const std::string args = has_args ? trim(command.substr(offset)) : std::string();
                    bool ignore_case = false;
                    bool touched = false;
                    std::string pattern;
                    parse_command_flags(args, &ignore_case, &touched, NULL, &pattern);
                    if (touched) {
                        print_touched_keys_filtered(last_touched_unchanged_keys, pattern, ignore_case);
                    }
                    else {
                        print_changed_keys_filtered(last_changed_keys, pattern, ignore_case);
                    }
                }
                handled_navigation = true;
            }
            else if (command == ":diff" || command == "diff" || starts_with(command, ":diff ") || starts_with(command, "diff ")) {
                const bool has_args = starts_with(command, ":diff ") || starts_with(command, "diff ");
                const size_t offset = command[0] == ':' ? 6 : 5;
                const std::string args = has_args ? trim(command.substr(offset)) : std::string();
                bool ignore_case = false;
                std::string pattern;
                parse_command_flags(args, &ignore_case, NULL, NULL, &pattern);
                std::vector<KeyChange> diff = compute_changed_scalar_keys(base_handle, h);
                diff = merge_declared_symbol_changes(diff, collect_declared_symbols_from_statements(session_statements), base_handle, h);
                print_changed_keys_filtered(diff, pattern, ignore_case);
                handled_navigation = true;
            }
            else if (command == ":list" || command == "list" || starts_with(command, ":list ") || starts_with(command, "list ")) {
                const bool has_args = starts_with(command, ":list ") || starts_with(command, "list ");
                const size_t offset = command[0] == ':' ? 6 : 5;
                const std::string args = has_args ? trim(command.substr(offset)) : std::string();
                bool ignore_case = false;
                bool values = false;
                std::string pattern;
                parse_command_flags(args, &ignore_case, NULL, &values, &pattern);
                print_keys(h, pattern, values, ignore_case);
                handled_navigation = true;
            }
            else if (command == ":accessors" || command == "accessors") {
                print_accessors();
                handled_navigation = true;
            }
            else if (starts_with(command, ":accessors ") || starts_with(command, "accessors ")) {
                const size_t offset = (command[0] == ':') ? 11 : 10;
                const std::string args = trim(command.substr(offset));
                bool ignore_case = false;
                std::string pattern;
                parse_command_flags(args, &ignore_case, NULL, NULL, &pattern);
                if (pattern.empty()) {
                    print_accessors();
                }
                else {
                    print_accessors(pattern, ignore_case);
                }
                handled_navigation = true;
            }
            else if (command == ":next" || command == "next") {
                handled_navigation = try_navigation(current_message + 1);
            }
            else if (command == ":prev" || command == "prev") {
                handled_navigation = try_navigation(current_message - 1);
            }
            else if (starts_with(command, ":goto ") || starts_with(command, "goto ")) {
                std::string num = trim(command.substr(command[0] == ':' ? 6 : 5));
                long requested = 0;
                if (!parse_positive_long(num, &requested)) {
                    fprintf(stderr, "codes_interpreter: invalid message index '%s'\n", num.c_str());
                    handled_navigation = true;
                }
                else {
                    handled_navigation = try_navigation(requested);
                }
            }
            else if (starts_with(command, ":save ") || starts_with(command, "save ")) {
                const size_t offset = (command[0] == ':') ? 6 : 5;
                const std::string path = trim(command.substr(offset));
                if (path.empty()) {
                    fprintf(stderr, "codes_interpreter: missing filename for :save\n");
                }
                else if (!write_text_file(path, session_script)) {
                    fprintf(stderr, "codes_interpreter: cannot write '%s'\n", path.c_str());
                }
                else {
                    printf("Saved session to %s\n", path.c_str());
                }
                handled_navigation = true;
            }
            else if (starts_with(command, ":load ") || starts_with(command, "load ")) {
                const size_t offset = (command[0] == ':') ? 6 : 5;
                const std::string path = trim(command.substr(offset));
                if (path.empty()) {
                    fprintf(stderr, "codes_interpreter: missing filename for :load\n");
                    handled_navigation = true;
                }
                else {
                    bool ok = false;
                    const std::string loaded = read_text_file(path, &ok);
                    if (!ok) {
                        fprintf(stderr, "codes_interpreter: cannot read '%s'\n", path.c_str());
                    }
                    else if (trim(loaded).empty()) {
                        printf("Loaded empty script from %s\n", path.c_str());
                    }
                    else {
                        grib_handle* next_handle = replay_session(base_handle, current_message, static_cast<long>(message_offsets.size()),
                                                                  session_script, loaded, &err);
                        if (!next_handle) {
                            fprintf(stderr, "codes_interpreter: %s\n", grib_get_error_message(err));
                        }
                        else {
                            std::vector<KeyChange> changed_now;
                            std::vector<KeyChange> touched_now;
                            if (log_key_changes) {
                                changed_now = compute_changed_scalar_keys(h, next_handle);
                                changed_now = merge_declared_symbol_changes(changed_now, extract_declared_symbols(loaded), h, next_handle);
                                touched_now = compute_touched_unchanged_scalar_keys(h, next_handle, changed_now);
                            }

                            grib_handle_delete(h);
                            h = next_handle;
                            grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
                            grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
                            if (log_key_changes) {
                                last_changed_keys.swap(changed_now);
                                last_touched_unchanged_keys.swap(touched_now);
                            }
                            else {
                                last_changed_keys.clear();
                                last_touched_unchanged_keys.clear();
                            }
#ifdef HAVE_LIBREADLINE
                            s_completion_handle = h;
                            const std::vector<std::string> declared_symbols = extract_declared_symbols(loaded);
                            for (const auto& declared_symbol : declared_symbols) {
                                s_session_symbols.insert(declared_symbol);
                            }
#endif
                            session_statements.push_back(loaded);
                            session_script = build_session_script(session_statements);
                            if (session_log.is_open() && log_key_changes) {
                                write_changed_keys(session_log, last_changed_keys);
                                session_log.flush();
                            }
                            printf("Loaded session from %s\n", path.c_str());
                        }
                    }
                    handled_navigation = true;
                }
            }
            else if (command == ":undo" || command == "undo") {
                if (session_statements.empty()) {
                    printf("Nothing to undo\n");
                }
                else {
                    const std::string removed = session_statements.back();
                    session_statements.pop_back();
                    const std::string rebuilt = build_session_script(session_statements);
                    grib_handle* next_handle = replay_session(base_handle, current_message, static_cast<long>(message_offsets.size()),
                                                              rebuilt, std::string(), &err);
                    if (!next_handle) {
                        session_statements.push_back(removed);
                        fprintf(stderr, "codes_interpreter: undo failed: %s\n", grib_get_error_message(err));
                    }
                    else {
                        std::vector<KeyChange> changed_now;
                        std::vector<KeyChange> touched_now;
                        if (log_key_changes) {
                            changed_now = compute_changed_scalar_keys(h, next_handle);
                            touched_now = compute_touched_unchanged_scalar_keys(h, next_handle, changed_now);
                        }
                        grib_handle_delete(h);
                        h = next_handle;
                        grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
                        grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
                        session_script = rebuilt;
                        if (log_key_changes) {
                            last_changed_keys.swap(changed_now);
                            last_touched_unchanged_keys.swap(touched_now);
                        }
                        else {
                            last_changed_keys.clear();
                            last_touched_unchanged_keys.clear();
                        }
#ifdef HAVE_LIBREADLINE
                        s_completion_handle = h;
                        s_session_symbols.clear();
                        for (const auto& persisted_statement : session_statements) {
                            const std::vector<std::string> declared_symbols = extract_declared_symbols(persisted_statement);
                            for (const auto& declared_symbol : declared_symbols) {
                                s_session_symbols.insert(declared_symbol);
                            }
                        }
#endif
                        if (session_log.is_open() && log_key_changes) {
                            write_changed_keys(session_log, last_changed_keys);
                            session_log.flush();
                        }
                        printf("Undid last statement\n");
                    }
                }
                handled_navigation = true;
            }
        }

        if (handled_navigation) {
            continue;
        }

        if (command.empty()) {
            continue;
        }

        script += text;

        if (is_complete_statement(script)) {
            std::string to_run = trim(script);
            if (!to_run.empty()) {
                const bool persist_statement = should_persist_statement(to_run);
                grib_handle* next_handle = replay_session(base_handle, current_message, static_cast<long>(message_offsets.size()),
                                                          session_script, to_run, &err);
                if (!next_handle) {
                    fprintf(stderr, "codes_interpreter: %s\n", grib_get_error_message(err));
                    if (non_fail) {
                        script.clear();
                        continue;
                    }
                    grib_handle_delete(base_handle);
                    grib_handle_delete(h);
                    return err;
                }
                std::vector<KeyChange> changed_now;
                std::vector<KeyChange> touched_now;
                if (log_key_changes && persist_statement) {
                    changed_now = compute_changed_scalar_keys(h, next_handle);
                    changed_now = merge_declared_symbol_changes(changed_now, extract_declared_symbols(to_run), h, next_handle);
                    touched_now = compute_touched_unchanged_scalar_keys(h, next_handle, changed_now);
                }
                grib_handle_delete(h);
                h = next_handle;
                grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
                grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
                if (log_key_changes && persist_statement) {
                    last_changed_keys.swap(changed_now);
                    last_touched_unchanged_keys.swap(touched_now);
                }
                else {
                    last_changed_keys.clear();
                    last_touched_unchanged_keys.clear();
                }
                if (session_log.is_open() && log_key_changes) {
                    write_changed_keys(session_log, last_changed_keys);
                    session_log.flush();
                }
#ifdef HAVE_LIBREADLINE
                s_completion_handle = h;
#endif
                if (persist_statement) {
#ifdef HAVE_LIBREADLINE
                    const std::vector<std::string> declared_symbols = extract_declared_symbols(to_run);
                    for (const auto& declared_symbol : declared_symbols) {
                        s_session_symbols.insert(declared_symbol);
                    }
#endif
                    session_statements.push_back(to_run);
                    session_script = build_session_script(session_statements);
                }
            }
            script.clear();
        }
#ifdef HAVE_LIBREADLINE
        if (command != "") {
            add_history(text.c_str());
        }
#endif
    }

    if (!script.empty()) {
        std::string to_run = trim(script);
        if (!to_run.empty()) {
            const bool persist_statement = should_persist_statement(to_run);
            grib_handle* next_handle = replay_session(base_handle, current_message, static_cast<long>(message_offsets.size()),
                                                      session_script, to_run, &err);
            if (!next_handle) {
                fprintf(stderr, "codes_interpreter: %s\n", grib_get_error_message(err));
                if (non_fail) {
                    grib_handle_delete(base_handle);
                    grib_handle_delete(h);
                    return 0;
                } else {
                    grib_handle_delete(base_handle);
                    grib_handle_delete(h);
                    return err;
                }
            }
            std::vector<KeyChange> changed_now;
            std::vector<KeyChange> touched_now;
            if (log_key_changes && persist_statement) {
                changed_now = compute_changed_scalar_keys(h, next_handle);
                changed_now = merge_declared_symbol_changes(changed_now, extract_declared_symbols(to_run), h, next_handle);
                touched_now = compute_touched_unchanged_scalar_keys(h, next_handle, changed_now);
            }
            grib_handle_delete(h);
            h = next_handle;
            grib_context_set_handle_file_count(h->context, static_cast<int>(current_message));
            grib_context_set_handle_total_count(h->context, static_cast<int>(message_offsets.size()));
            if (log_key_changes && persist_statement) {
                last_changed_keys.swap(changed_now);
                last_touched_unchanged_keys.swap(touched_now);
            }
            else {
                last_changed_keys.clear();
                last_touched_unchanged_keys.clear();
            }
            if (session_log.is_open() && log_key_changes) {
                write_changed_keys(session_log, last_changed_keys);
                session_log.flush();
            }
#ifdef HAVE_LIBREADLINE
            s_completion_handle = h;
#endif
            if (persist_statement) {
                session_statements.push_back(to_run);
                session_script = build_session_script(session_statements);
#ifdef HAVE_LIBREADLINE
                const std::vector<std::string> declared_symbols = extract_declared_symbols(to_run);
                for (const auto& declared_symbol : declared_symbols) {
                    s_session_symbols.insert(declared_symbol);
                }
#endif
            }
        }
    }

    grib_handle_delete(base_handle);
    grib_handle_delete(h);
    return 0;
}
