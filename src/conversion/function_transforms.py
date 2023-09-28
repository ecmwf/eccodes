# Function transforms to apply to all converted C code

import re
from convert_debug import debug_line

c_lib_substitutions = {
    r"\bstrcmp\((.*),\s*(.*)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
    r"\bstrlen\(\s*(.*)\s*\)": r"\1.size()",
    # snprintf substitutions can span multiple lines, but we only need to match to the start of the format string...
    # This version matches either an explicit size or sizeof(x)
    r"\bsnprintf\((\w+),\s*(?:sizeof\(\w*\))?(?:\d+)?\s*,\s*(\")": r"\1 = fmtString(\2",
}

# Note - these substitutions are applied in the order defined below, so dependencies
#        can be used if required...
grib_get_set_substitutions = {
    # grib_ functions
    # Note: 1. We treat e.g. grib_get_long and grib_get_long_internal the same...
    #       2. The first argument (h) may already be stripped so we optionally match it
    #       3. The second argument may be a string literal, which needs to convert to an AccessorName object
    #
    # First, let's convert the second argument to an AccessorName if required
    r"\b(grib_[gs]et_\w+)(_internal)?\(\s*(h\s*,\s*)?\s*(\".*\")": r"\1\2(\3AccessorName(\4)",
    # Now, complete the conversion - note we remove any references
    r"\b([\w\s]+)\s*=\s*grib_get_long(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = unpackLong(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_get_double(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = unpackDouble(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_get_string(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*),\s*(.*)\s*\)": r"\1 = unpackString(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_set_long(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = packLong(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_set_double(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = packDouble(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_set_string(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*),\s*(.*)\s*\)": r"\1 = packString(\4, \5)",
}

grib_iarray_substitutions = {
    r"\bgrib_iarray_new\(\s*(h\s*,\s*)?\s*(.*)?,\s*(.*)?\s*\)": r"std::vector<long>(\2)",
}
    
def apply_all_func_transforms(line):
    for k, v in c_lib_substitutions.items():
        line, count = re.subn(k, v, line)
        if count:
            debug_line("apply_all_func_transforms", f"c_lib substitutions [after ]: {line}")

    for k, v in grib_get_set_substitutions.items():
        line, count = re.subn(k, v, line)
        if count:
            debug_line("apply_all_func_transforms", f"grib get/set substitutions [after ]: {line}")

    for k, v in grib_iarray_substitutions.items():
        line, count = re.subn(k, v, line)
        if count:
            debug_line("apply_all_func_transforms", f"grib iarray substitutions [after ]: {line}")

    return line
