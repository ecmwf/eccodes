
import utils.debug as debug
import code_object.arg as arg
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class ArgConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, arg.Arg), f"Expected arg.Arg, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_decl_spec = conversion_funcs.convert_ccode_object(self._ccode_object.decl_spec, conversion_data)
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_data)
        cpp_is_func_arg = self._ccode_object.is_func_arg

        return arg.Arg(cpp_decl_spec, cpp_name, cpp_is_func_arg)
