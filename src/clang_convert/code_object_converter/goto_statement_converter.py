
import utils.debug as debug
import code_object.goto_statement as goto_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class GotoStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, goto_statement.GotoStatement), f"Expected GotoStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_label = conversion_funcs.convert_ccode_object(self._ccode_object.label, conversion_pack)

        cppgoto_statement = goto_statement.GotoStatement(cpp_label)
        return conversion_pack.conversion_validation.validate_goto_statement(self._ccode_object, cppgoto_statement)
