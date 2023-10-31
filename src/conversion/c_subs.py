# C code substitutions to apply

import re
import debug

c_lib_substitutions = {
    r"\bstrcmp\((.*),\s*(.*)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
    r"\bstrlen\(\s*([^\)]*)\s*\)": r"\1.size()",
    # snprintf substitutions can span multiple lines, but we only need to match to the start of the format string...
    # This version matches either an explicit size or sizeof(x)
    r"\bsnprintf\((\w+),\s*(?:sizeof\(\w*\))?(?:\d+)?\s*,\s*(\")": r"\1 = fmtString(\2",
    r"\bstrtol": "stringToLong",
    r"\bstrtod": "stringToDouble",
}

# grib_ functions that can be deleted - we'll comment them out for now!
grib_deleted_function_substitutions = {
    r"^\s*(.*?\bgrib_context_malloc_clear)": r"// [Removed grib_context_malloc_clear] \1",
    r"^\s*(.*?\bgrib_context_free)": r"// [Removed grib_context_free] \1",
}

grib_context_substitutions = {
    r"\bgrib_context_log\(.*?,": r"gribLog(",
}

def apply_all_substitutions(line):

    func_substitutions = [
        c_lib_substitutions,
        grib_deleted_function_substitutions,
        grib_context_substitutions
    ]

    for func_substitution_dict in func_substitutions:
        for k, v in func_substitution_dict.items():
            line, count = re.subn(k, v, line)
            if count:
                debug.line("apply_all_substitutions", f"Updated line: {line}")

    return line
