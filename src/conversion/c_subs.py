# C code substitutions to apply

import re
import debug

def sscanf_repl(m):
    return f"scanString({m.group(1)}, {m.group(3) if m.group(3) else '0'}{m.group(4)})"

c_lib_substitutions = {
    r"\bstrcmp\(([^,]+),\s*([^\)]+)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
    r"\bstrlen\(\s*([^\)]*)\s*\)": r"\1.size()",
    # snprintf substitutions can span multiple lines, but we only need to match to the start of the format string...
    # This version matches either an explicit size or sizeof(x)
    r"\bsnprintf\(([^,]+),[^,]+,": r"\1 = fmtString(",
    r"\bstrtol": "strToLong",
    r"\bstrtod": "strToDouble",
    r"\batol": "strAtoL",
    r"\bstrcpy\(([^,]+),([^\)]+)\)": r"\1 = \2",
    r"\bstrcat\(([^,]+),([^\)]+)\)": r"\1 += \2",
    r"\bsscanf\(([^\s,\)]+)(\s+\+\s+([^,]+))?([^\)]*)\)": sscanf_repl,
    r"\bmemcpy\(([^,]+),([^,]+),([^\)]+)\)": r"copyBuffer(\1,\2,\3)",
    r"\bstrncpy\(([^,]+),([^,]+),([^\)]+)\)": r"copyString(\1,\2,\3)",
    r"\bfgets": r"fgetsFromString",
}

# grib_ functions that can be deleted - we'll comment them out for now!
grib_deleted_function_substitutions = {
    r"^\s*(.*?\bgrib_context_free)": r"// [Removed grib_context_free] \1",
    r"^\s*(.*?\bgrib_context_free_persistent)": r"// [Removed grib_context_free_persistent] \1",
    r"^\s*(.*?\bgrib_context_buffer_free)": r"// [Removed grib_context_buffer_free] \1",
}

grib_context_substitutions = {
    r"\bgrib_context_log\(.*?,": r"gribLog(",
}

codes_substitutions = {
    r"\bcodes_fopen\b": r"fopenFromString",
}


def apply_all_substitutions(line):

    func_substitutions = [
        c_lib_substitutions,
        grib_deleted_function_substitutions,
        grib_context_substitutions,
        codes_substitutions,
    ]

    for func_substitution_dict in func_substitutions:
        for k, v in func_substitution_dict.items():
            line, count = re.subn(k, v, line)
            if count:
                debug.line("apply_all_substitutions", f"Updated line: {line}")

    return line
