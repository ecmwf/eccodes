
import utils.debug as debug
import code_object.catch_statement as catch_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class CatchStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, catch_statement.CatchStatement), f"Expected CatchStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_expression = conversion_funcs.convert_ccode_object(self._ccode_object.expression, conversion_pack)
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        cppcatch_statement = catch_statement.CatchStatement(cpp_expression, cpp_statement)
        return conversion_pack.conversion_validation.validate_catch_statement(self._ccode_object, cppcatch_statement)
