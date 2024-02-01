
import utils.debug as debug
import code_object.macro_instantation as macro_instantation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class MacroInstantiationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, macro_instantation.MacroInstantation), f"Expected MacroInstantation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_macro = self._ccode_object
        cpp_macro_expression = conversion_funcs.convert_ccode_object(cpp_macro.expression, conversion_pack)

        cpp_macro_inst = macro_instantation.MacroInstantation(cpp_macro.name, cpp_macro_expression)
        
        return conversion_pack.conversion_validation.validate_macro_instantiation(self._ccode_object, cpp_macro_inst)
