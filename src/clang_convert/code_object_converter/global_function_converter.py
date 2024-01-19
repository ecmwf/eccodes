
import utils.debug as debug
import code_object.global_function as global_function
import code_object_converter.function_converter as function_converter
import code_object_converter.conversion_funcs as conversion_funcs

class GlobalFunctionConverter(function_converter.FunctionConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, global_function.GlobalFunction), f"Expected GlobalFunction, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_body = conversion_funcs.convert_ccode_object(self._ccode_object.body, conversion_data)

        return global_function.GlobalFunction(cpp_body)
  