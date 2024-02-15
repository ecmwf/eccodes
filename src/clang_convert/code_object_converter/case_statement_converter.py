
import utils.debug as debug
import code_object.case_statement as case_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class CaseStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, case_statement.CaseStatement), f"Expected CaseStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_constant_expression = conversion_funcs.convert_ccode_object(self._ccode_object.constant_expression, conversion_pack)
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        cpp_case = case_statement.CaseStatement(cpp_constant_expression, cpp_statement)

        return conversion_pack.conversion_validation.validate_case_statement(self._ccode_object, cpp_case)
