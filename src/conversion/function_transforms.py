# Function transforms to apply to all converted C code

import re
from convert_debug import debug_line

c_lib_substitutions = {
    r"\bstrcmp\((.*),\s*(.*)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
    r"\bstrlen\(\s*(.*)\s*\)": r"\1.size()",
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

grib_iarray_substitutions = {
    r"\bgrib_iarray_new\(\s*(h\s*,\s*)?\s*(.*)?,\s*(.*)?\s*\)": r"std::vector<long>(\2)",
    r"\bgrib_iarray_push\(\s*(.*)?,\s*(.*)?\s*\)": r"\1.push_back(\2)",
    r"\bgrib_iarray_used_size\(\s*(.*)?\s*\)": r"\1.size()",
    r"\bgrib_iarray_get_array\(\s*(.*)?\s*\)": r"\1",
    r"^\s*(.*?\bgrib_iarray_delete)": r"// [Removed grib_iarray_delete] \1",
}
    
def apply_all_func_transforms(line):

    func_substitutions = [
        c_lib_substitutions,
        grib_deleted_function_substitutions,
        grib_context_substitutions,
        grib_iarray_substitutions
    ]

    for func_substitution_dict in func_substitutions:
        for k, v in func_substitution_dict.items():
            line, count = re.subn(k, v, line)
            if count:
                debug_line("apply_all_func_transforms", f"Updated line: {line}")

    return line
