
import utils.debug as debug
import code_object.literal as literal
import code_object.function_call as function_call
from code_object_converter.conversion_funcs import as_commented_out_code

special_function_name_mapping = {
    "snprintf"                      : "fmtString",
    "strtol"                        : "strToLong",
    "strtod"                        : "strToDouble",
    "atol"                          : "strAtoL",
    "fgets"                         : "fgetsFromString",
    "grib_context_log"              : "gribLog",
    "codes_fopen"                   : "fopenFromString",
    "memcpy"                        : "copyBuffer",
    "strncpy"                       : "copyString",
    # Deleted functions
    "grib_context_free"             : "",
    "grib_context_free_persistent"  : "",
    "grib_context_buffer_free"      : "",
}

def apply_special_function_call_conversions(cfunction_call, cppfunction_call):

        if cfunction_call.name == "grib_arguments_get_name":
            arg_entry = literal.Literal(f"initData.args[{cfunction_call.args[2].as_string()}].second")
            return function_call.FunctionCall(f"std::get<std::string>", [arg_entry])

        if cfunction_call.name == "grib_arguments_get_long":
            arg_entry = literal.Literal(f"initData.args[{cfunction_call.args[2].as_string()}].second")
            return function_call.FunctionCall(f"std::get<long>", [arg_entry])

        for cfuncname, cppfuncname in special_function_name_mapping.items():
            if cfunction_call.name == cfuncname:
                if cppfuncname:
                    return function_call.FunctionCall(cppfuncname, cppfunction_call.args)
                else:
                    return as_commented_out_code(cfunction_call, f"Removed call to {cfuncname}")

        return None
