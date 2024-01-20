
import utils.debug as debug
import code_object.function_call as function_call
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class FunctionCallConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, function_call.FunctionCall), f"Expected FunctionCall, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_data)
        cpp_args = []
        for arg_entry in self._ccode_object.args:
            cpp_arg_entry = conversion_funcs.convert_ccode_object(arg_entry, conversion_data)
            cpp_args.append(cpp_arg_entry)

        return function_call.FunctionCall(cpp_name, cpp_args)
