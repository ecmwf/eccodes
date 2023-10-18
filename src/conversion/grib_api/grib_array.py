import re
import debug

grib_array_type_conversions = {
    "darray": "std::vector<double>",
    "iarray": "std::vector<long>",
    "sarray": "std::string",
    "vdarray": "std::vector<std::vector<double>>",
    "viarray": "std::vector<std::vector<long>>",
    "vsarray": "std::vector<std::string>",
}

grib_xarray_substitutions = {
    r"\bgrib_v?[dis]array_push\(\s*(.*)?,\s*(.*)?\s*\)": r"\1.push_back(\2)",
    r"\bgrib_v?[dis]array_used_size\(\s*(.*)?\s*\)": r"\1.size()",
    r"\bgrib_v?[dis]array_get_array\(\s*(.*)?\s*\)": r"\1",
    r"^\s*(.*?\bgrib_v?[dis]array_delete)": r"// [Removed grib_xarray_delete] \1",
}
    
def convert_grib_array_functions(line):
    m = re.search(r"\bgrib_(v?[dis]iarray)_new\(\s*(h\s*,\s*)?\s*(.*)?,\s*(.*)?\s*\)", line)
    if m:
        line = re.sub(m.re, f"{grib_array_type_conversions[m.group(1)]({m.group(3)})}", line)
        debug.line("convert_grib_array_functions", f"Updated line: {line}")

    for k, v in grib_xarray_substitutions.items():
        line, count = re.subn(k, v, line)
        if count:
            debug.line("convert_grib_array_functions", f"Updated line: {line}")

    return line