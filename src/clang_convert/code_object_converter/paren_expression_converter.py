
import utils.debug as debug
import code_object.paren_expression as paren_expression
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class ParenExpressionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, paren_expression.ParenExpression), f"Expected ParenExpression, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_data)

        return paren_expression.ParenExpression(cpp_expression)
