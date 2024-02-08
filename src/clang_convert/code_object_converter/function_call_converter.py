
import utils.debug as debug
import code_object.function_call as function_call
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.arg as arg
from code_object.code_interface import NONE_VALUE

class FunctionCallConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, function_call.FunctionCall), f"Expected FunctionCall, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cfunction_call = self._ccode_object
        cppfunction_call = None

        # 1. Check if there is a function mapping defined
        mapping = conversion_pack.conversion_data.funcsig_mapping_for_cfuncname(cfunction_call.name)
        if mapping:
            debug.line("create_cpp_code_object", f"FunctionCallConverter [1] mapping.cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] -> mapping.cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")
            cpp_args = []
            for i, arg_entry in enumerate(mapping.cppfuncsig.args):
                debug.line("create_cpp_code_object", f"FunctionCallConverter [1] --> i=[{i}] arg_entry=[{debug.as_debug_string(arg_entry)}]")
                if arg_entry != NONE_VALUE:
                    cpp_arg_entry = conversion_funcs.convert_ccode_object(cfunction_call.args[i], conversion_pack)
                    assert cpp_arg_entry != NONE_VALUE, f"Expected cpp_arg_entry for carg=[{debug.as_debug_string(cfunction_call.args[i])}], got NoneValue!"
                    cpp_args.append(cpp_arg_entry)

            cppfunction_call = function_call.FunctionCall(mapping.cppfuncsig.name, cpp_args)
        else:
            debug.line("create_cpp_code_object", f"FunctionCallConverter [2]")
            # 2. Perform a manual conversion
            cpp_name = conversion_funcs.convert_ccode_object(cfunction_call.name, conversion_pack)
            cpp_args = []
            for arg_entry in self._ccode_object.args:
                cpp_arg_entry = conversion_funcs.convert_ccode_object(arg_entry, conversion_pack)

                if cpp_arg_entry and cpp_arg_entry != NONE_VALUE:
                    cpp_args.append(cpp_arg_entry)

            cppfunction_call = function_call.FunctionCall(cpp_name, cpp_args)

        # 3. Apply validation (and special handling)
        updated_cppfunction_call = conversion_pack.conversion_validation.validate_function_call(cfunction_call, cppfunction_call)

        # 4. Add the function call to the conversion data in case we need to process it later (e.g. for using C::x declarations)
        # Only if function_call type!
        if isinstance(updated_cppfunction_call, function_call.FunctionCall):
            conversion_pack.conversion_data.add_cppfunction_call(updated_cppfunction_call)

        return updated_cppfunction_call
