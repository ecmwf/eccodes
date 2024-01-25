
import utils.debug as debug
import code_object.unary_expression as unary_expression
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class UnaryExpressionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, unary_expression.UnaryExpression), f"Expected UnaryExpression, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_keyword = conversion_funcs.convert_ccode_object(self._ccode_object.keyword, conversion_pack)
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)

        return unary_expression.UnaryExpression(cpp_keyword, cpp_expression)
