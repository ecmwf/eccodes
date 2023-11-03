
import grib_api.grib_query_funcsig_conv as grib_query_funcsig_conv
import grib_api.grib_value_funcsig_conv as grib_value_funcsig_conv
import re
import debug

funcsig_conversions = [
    grib_query_funcsig_conv.grib_query_funcsig_conversions,
    grib_value_funcsig_conv.grib_value_funcsig_conversions
]

def convert_funcsig(line):
    m = re.search(r"\b(grib[^(\s]*)\(", line)
    if m:
        func_name = m.group(1)
        cppfuncsig = None
        
        for funcsig_conv in funcsig_conversions:
            for mapping in funcsig_conv:
                if mapping.cfuncsig.name == func_name:
                    cppfuncsig = mapping.cppfuncsig
                    break

        if not cppfuncsig:
            return line

        match_start = m.start()
        match_end = m.end()

        # Capture the parameters
        params = []
        param_re = r"\s*([^,]*)\s*"
        while match_end < len(line):
            m = re.search(rf"{param_re},", line[match_end:])
            if m:
                match_end += m.end()
                params.append(m.group(1))
            else:
                break

        # Final param...
        param_re = r"\s*([^\)]*)\s*"
        m = re.search(rf"{param_re}\)", line[match_end:])
        if m:
            params.append(m.group(1))
            match_end += m.end()
        else:
            debug.line("convert_funcsig", f"No final param - is it multi-line? Input line [{line}]")

        converted_params = convert_params(cppfuncsig, params)

        converted_func = f"{cppfuncsig.name}({','.join([p for p in converted_params])})"

        line = line[:match_start] + converted_func + line[match_end:]

        debug.line("convert_funcsig",f"Converted function [{func_name}] [After]: {line}")

    return line

def convert_params(cppfuncsig, params):
    converted_params = []

    for index, func_arg in enumerate(cppfuncsig.args):
        if not func_arg:
            continue

        param = params[index]

        if func_arg.type.startswith("AccessorName"):
            if param[0] == "\"":
                param = "AccessorName(" + param + ")"

        if param[0] == "&":
            param = param[1:]

        converted_params.append(param)
    
    return converted_params

