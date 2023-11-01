import re
import debug
from grib_api.grib_type_transforms import grib_array_function_transforms
import struct_arg


grib_xarray_substitutions = {
    r"\bgrib_v?[dis]array_push\(\s*(.*)?,\s*(.*)?\s*\)": r"\1.push_back(\2)",
    r"\bgrib_v?[dis]array_used_size\(\s*(.*)?\s*\)": r"\1.size()",
    r"\bgrib_v?[dis]array_get_array\(\s*(.*)?\s*\)": r"\1",
    r"^\s*(.*?\bgrib_v?[dis]array_delete)": r"// [Removed grib_xarray_delete] \1",
}
    
def convert_grib_array_functions(line):
    m = re.search(r"\b(grib_v?[dis]array)_new\(\s*(h\s*,\s*)?\s*(.*)?,\s*(.*)?\s*\)", line)
    if m:
        line = re.sub(m.re, f"{grib_array_function_transforms[m.group(1)]}({m.group(3)})", line)
        debug.line("convert_grib_array_functions", f"Updated line: {line}")

    for k, v in grib_xarray_substitutions.items():
        line, count = re.subn(k, v, line)
        if count:
            debug.line("convert_grib_array_functions", f"Updated line: {line}")

    return line

# By default, replace s->v[4]->v[5] with s[4][5], and any other -> with .
def process_grib_array_cstruct_arg(cstruct_arg, cppname):
    debug.line("process_grib_array_cstruct_arg", f"cstruct_arg={cstruct_arg.as_string()} cppname={cppname}")
    cppstruct_arg = struct_arg.StructArg("", cppname, cstruct_arg.index)

    cmember = cstruct_arg.member
    cppmember = cppstruct_arg
    while cmember:
        if cmember.access == "->":
            if cmember.name == "v":
                cppmember.member = struct_arg.StructArg("", "", cmember.index)
            else:
                cppmember.member = struct_arg.StructArg(".", cmember.name , cmember.index)
        else:
            cppmember.member = struct_arg.StructArg(cmember.access, cmember.name , cmember.index)

        cmember = cmember.member
        cppmember = cppmember.member

    return cppstruct_arg