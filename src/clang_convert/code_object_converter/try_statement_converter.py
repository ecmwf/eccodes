
import utils.debug as debug
import code_object.try_statement as try_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class TryStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, try_statement.TryStatement), f"Expected TryStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        cpptry_statement = try_statement.TryStatement(cpp_statement)
        return conversion_pack.conversion_validation.validate_try_statement(self._ccode_object, cpptry_statement)
