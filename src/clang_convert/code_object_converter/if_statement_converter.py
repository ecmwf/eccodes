
import utils.debug as debug
import code_object.if_statement as if_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class IfStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, if_statement.IfStatement), f"Expected IfStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_data)
        cpp_action = conversion_funcs.convert_ccode_object(self._ccode_object.action, conversion_data)

        return if_statement.IfStatement(cpp_expression, cpp_action)
