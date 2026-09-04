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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static std::string find_definitions_path(const char* argv0)
{
    std::vector<std::string> candidates;

    candidates.push_back("./definitions");
    candidates.push_back("/ec/res4/scratch/maro/ecc_codes_interpreter/eccodes/definitions");

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

static grib_handle* replay_session(const grib_handle* base_handle, const std::string& session_script, const std::string& statement, int* err)
{
    grib_handle* trial_handle = grib_handle_clone(base_handle);
    if (!trial_handle) {
        if (err)
            *err = GRIB_OUT_OF_MEMORY;
        return NULL;
    }

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
    fprintf(stderr, "Usage: %s [--non-fail|-n] [--help|-h] <message_file>\n", program);
    fprintf(stderr, "Open one GRIB/BUFR/GTS message and evaluate ecCodes filter statements from standard input.\n");
    fprintf(stderr, "  --non-fail, -n  Keep the interpreter open after a statement fails\n");
    fprintf(stderr, "  --help, -h      Show this help message\n");
}

int main(int argc, char* argv[])
{
    bool non_fail = false;
    int file_arg = -1;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--non-fail" || arg == "-n") {
            non_fail = true;
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

    const char* env_defs = getenv("ECCODES_DEFINITION_PATH");
    const char* env_legacy_defs = getenv("GRIB_DEFINITION_PATH");
    if ((!env_defs || env_defs[0] == '\0') && (!env_legacy_defs || env_legacy_defs[0] == '\0')) {
        std::string defs = find_definitions_path(argv[0]);
        if (!defs.empty()) {
            setenv("ECCODES_DEFINITION_PATH", defs.c_str(), 1);
            grib_context_set_definitions_path(grib_context_get_default(), defs.c_str());
        }
    }

    FILE* input = fopen(argv[file_arg], "rb");
    if (!input) {
        fprintf(stderr, "codes_interpreter: cannot open message file '%s'\n", argv[file_arg]);
        return 1;
    }

    int err = 0;
    grib_handle* h = grib_handle_new_from_file(grib_context_get_default(), input, &err);
    fclose(input);

    if (!h) {
        fprintf(stderr, "codes_interpreter: cannot decode message '%s': %s\n", argv[file_arg], grib_get_error_message(err));
        return 1;
    }

    grib_handle* base_handle = grib_handle_clone(h);
    if (!base_handle) {
        fprintf(stderr, "codes_interpreter: cannot clone message '%s'\n", argv[file_arg]);
        grib_handle_delete(h);
        return GRIB_OUT_OF_MEMORY;
    }

    std::string script;
    std::string session_script;
    char line[4096];

    printf("\n=== codes_interpreter started ===\n");
    printf("ecCodes version %d.%d.%d\n\n", ECCODES_MAJOR_VERSION, ECCODES_MINOR_VERSION, ECCODES_REVISION_VERSION);
    printf("Message: %s\n", argv[file_arg]);
    printf("Type a filter expression and end with ';' or type 'quit' to exit.\n");

#ifdef HAVE_LIBREADLINE
    using_history();
    rl_bind_key('\t', rl_insert);
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

        std::string command = trim(text);
        if (command == "quit" || command == "exit") {
            break;
        }

        if (command.empty()) {
#ifdef HAVE_LIBREADLINE
            continue;
#else
            continue;
#endif
        }

        script += text;

        if (is_complete_statement(script)) {
            std::string to_run = trim(script);
            if (!to_run.empty()) {
                grib_handle* next_handle = replay_session(base_handle, session_script, to_run, &err);
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
                grib_handle_delete(h);
                h = next_handle;
                if (should_persist_statement(to_run)) {
                    session_script += to_run;
                    session_script += "\n";
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
            grib_handle* next_handle = replay_session(base_handle, session_script, to_run, &err);
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
            grib_handle_delete(h);
            h = next_handle;
        }
    }

    grib_handle_delete(base_handle);
    grib_handle_delete(h);
    return 0;
}
