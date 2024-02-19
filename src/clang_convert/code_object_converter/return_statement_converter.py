
import utils.debug as debug
import code_object.return_statement as return_statement
import code_object.literal as literal
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE

class ReturnStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, return_statement.ReturnStatement), f"Expected ReturnStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)

        if cpp_expression == NONE_VALUE:
            debug.line("create_cpp_code_object", "RETURN STATEMENT - WARNING: cpp_expression=[NONE_VALUE] so setting default: [return {};]")
            return return_statement.ReturnStatement(literal.Literal("{}"))

        cppreturn_statement = return_statement.ReturnStatement(cpp_expression)
        return conversion_pack.conversion_validation.validate_return_statement(self._ccode_object, cppreturn_statement)
