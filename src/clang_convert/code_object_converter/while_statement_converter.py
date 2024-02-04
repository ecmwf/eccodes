
import utils.debug as debug
import code_object.while_statement as while_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class WhileStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, while_statement.WhileStatement), f"Expected WhileStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)
        cpp_action = conversion_funcs.convert_ccode_object(self._ccode_object.action, conversion_pack)

        cpp_while = while_statement.WhileStatement(cpp_expression, cpp_action)

        return conversion_pack.conversion_validation.validate_while_statement(self._ccode_object, cpp_while)
