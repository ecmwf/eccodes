
import utils.debug as debug
import code_object.literal as literal
import code_object.function_call as function_call

def apply_special_function_call_conversions(cfunction_call, cppfunction_call):

        if cfunction_call.name == "grib_arguments_get_name":
            arg_entry = literal.Literal(f"initData.args[{cfunction_call.args[2].as_string()}].second")
            return function_call.FunctionCall(f"std::get<std::string>", [arg_entry])

        if cfunction_call.name == "grib_arguments_get_long":
            arg_entry = literal.Literal(f"initData.args[{cfunction_call.args[2].as_string()}].second")
            return function_call.FunctionCall(f"std::get<long>", [arg_entry])

        if cfunction_call.name == "snprintf":
            return function_call.FunctionCall("fmtString", cppfunction_call.args)

        return None
