
import utils.debug as debug
import code_object.label_statement as label_statement
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class LabelStatementConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, label_statement.LabelStatement), f"Expected LabelStatement, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_label = conversion_funcs.convert_ccode_object(self._ccode_object.label, conversion_pack)
        cpp_statement = conversion_funcs.convert_ccode_object(self._ccode_object.statement, conversion_pack)

        cpp_label_stmt = label_statement.LabelStatement(cpp_label, cpp_statement)

        return conversion_pack.conversion_validation.validate_label_statement(self._ccode_object, cpp_label_stmt)
