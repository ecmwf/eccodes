
import utils.debug as debug
import code_object.cast_expression as cast_expression
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE
from code_object_converter.conversion_utils import as_commented_out_code

class CastExpressionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, cast_expression.CastExpression), f"Expected CastExpression, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        # By default, just switch to static_cast
        cpp_cast_type = "static"
        cpp_cast_value = conversion_funcs.convert_ccode_object(self._ccode_object.cast_value, conversion_pack)
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)

        if cpp_expression == NONE_VALUE:
            debug.line("create_cpp_code_object", f"cpp_expression=[{debug.as_debug_string(cpp_expression)}] => returning [NONE_VALUE]")
            return NONE_VALUE


        cppcast_expression = cast_expression.CastExpression(cpp_cast_type, cpp_cast_value, cpp_expression)
        return conversion_pack.conversion_validation.validate_cast_expression(self._ccode_object, cppcast_expression)
