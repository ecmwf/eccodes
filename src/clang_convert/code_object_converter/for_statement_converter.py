
import utils.debug as debug
import code_object.for_statement as for_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class ForStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, for_statement.ForStatement), f"Expected ForStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_init_statement = conversion_funcs.convert_ccode_object(self._ccode_object.init_statement, conversion_pack)
        cpp_condition = conversion_funcs.convert_ccode_object(self._ccode_object.condition, conversion_pack)
        cpp_iteration_expression = conversion_funcs.convert_ccode_object(self._ccode_object.iteration_expression, conversion_pack)
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        return for_statement.ForStatement(cpp_init_statement, cpp_condition, cpp_iteration_expression, cpp_statement)
  