
import utils.debug as debug
import code_object.function as function
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class FunctionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, function.Function), f"Expected Function, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_funcsig = conversion_funcs.convert_ccode_object(self._ccode_object.funcsig, conversion_pack)
        cpp_body = conversion_funcs.convert_ccode_object(self._ccode_object.body, conversion_pack)

        return function.Function(cpp_funcsig, cpp_body)
  