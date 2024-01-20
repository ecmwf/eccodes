
import utils.debug as debug
import code_object.macro_definition as macro_definition
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class MacroDefinitionConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, macro_definition.MacroDefinition), f"Expected MacroDefinition, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        # For now, just return a copy...
        return macro_definition.MacroDefinition(self._ccode_object.as_lines())
