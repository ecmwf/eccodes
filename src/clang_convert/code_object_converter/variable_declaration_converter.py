
import utils.debug as debug
import code_object.variable_declaration as variable_declaration
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class VariableDeclarationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, variable_declaration.VariableDeclaration), f"Expected VariableDeclaration, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data

        cpp_variable = conversion_funcs.convert_ccode_object(self._ccode_object.variable, conversion_data)
        cpp_value = conversion_funcs.convert_ccode_object(self._ccode_object.value, conversion_data)

        cpp_variable_declaration = variable_declaration.VariableDeclaration(cpp_variable, cpp_value)
        return cpp_variable_declaration


  