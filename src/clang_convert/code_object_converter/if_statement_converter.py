
import utils.debug as debug
import code_object.if_statement as if_statement
import code_object.literal as literal
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE

class IfStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, if_statement.IfStatement), f"Expected IfStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)
        cpp_action = conversion_funcs.convert_ccode_object(self._ccode_object.action, conversion_pack)

        if cpp_expression == NONE_VALUE:
            debug.line("create_cpp_code_object", f"C expression=[{debug.as_debug_string(self._ccode_object.expression)}] converted to NONE_VALUE: setting to false")
            cpp_expression = literal.Literal("false")

        cpp_else = conversion_funcs.convert_ccode_object(self._ccode_object.else_statement, conversion_pack)

        cpp_if = if_statement.IfStatement(cpp_expression, cpp_action, cpp_else)

        return conversion_pack.conversion_validation.validate_if_statement(self._ccode_object, cpp_if)
