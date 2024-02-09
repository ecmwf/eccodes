
import utils.debug as debug
import code_object.function as function
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE

class FunctionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, function.Function), f"Expected Function, got type=[{type(ccode_object)}]"

    # NOTE: Subclasses should be careful overriding this function as it controls whether the conversion
    # is valid, and returns NONE_VALUE is not!
    # Override the funcsig and bod creators instead
    def create_cpp_code_object(self, conversion_pack):

        cpp_funcsig = self.create_cpp_funcsig(conversion_pack)
        if cpp_funcsig == NONE_VALUE:
            debug.line("create_cpp_code_object", f"Function name=[{self._ccode_object.funcsig.name}] cannot be converted: cpp_funcsig=[NONE_VALUE]")
            return NONE_VALUE

        cpp_body = self.create_cpp_body(conversion_pack)

        cpp_function = self.create_cpp_function(cpp_funcsig, cpp_body, conversion_pack)

        if cpp_function != NONE_VALUE:
            cpp_function = self.validate_cpp_function(cpp_function, conversion_pack)

        return cpp_function
  
    # ---------- Override the following classes as required ----------

    def create_cpp_funcsig(self, conversion_pack):
        return conversion_funcs.convert_ccode_object(self._ccode_object.funcsig, conversion_pack)

    def create_cpp_body(self, conversion_pack):
        return conversion_funcs.convert_ccode_object(self._ccode_object.body, conversion_pack)

    # Override to create the correct function class
    def create_cpp_function(self, cpp_funcsig, cpp_body, conversion_pack):
        return function.Function(cpp_funcsig, cpp_body)

    # Override to call the correct validation function
    def validate_cpp_function(self, cpp_function, conversion_pack):
        return conversion_pack.conversion_validation.validate_function(self._ccode_object, cpp_function)
