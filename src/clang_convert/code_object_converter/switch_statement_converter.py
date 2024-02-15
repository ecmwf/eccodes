
import utils.debug as debug
import code_object.switch_statement as switch_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class SwitchStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, switch_statement.SwitchStatement), f"Expected SwitchStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_condition = conversion_funcs.convert_ccode_object(self._ccode_object.condition, conversion_pack)
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        debug.line("create_cpp_code_object", f"cpp_condition=[{debug.as_debug_string(cpp_condition)}] for self._ccode_object.condition=[{debug.as_debug_string(self._ccode_object.condition)}]")
        cpp_while = switch_statement.SwitchStatement(cpp_condition, cpp_statement)

        return conversion_pack.conversion_validation.validate_while_statement(self._ccode_object, cpp_while)
