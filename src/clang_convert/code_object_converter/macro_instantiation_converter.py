
import utils.debug as debug
import code_object.macro_instantation as macro_instantation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.code_objects as code_objects
import code_object.literal as literal
from code_object.code_interface import NONE_VALUE

class MacroInstantiationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, macro_instantation.MacroInstantation), f"Expected MacroInstantation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cmacro = self._ccode_object

        # We convert each entry in the macro expansion individually, so we can handle anything unexpected!
        cpp_macro_expression = code_objects.CodeObjects()

        for entry in cmacro.expression.code_objects:
            cppentry = conversion_funcs.convert_ccode_object(entry, conversion_pack)

            if cppentry == NONE_VALUE:
                debug.line("create_cpp_code_object", f"Unexpected [NONE_VALUE] in macro - commenting out")
                cppentry = literal.Literal(f"/* {entry.as_string()} */")
            
            cpp_macro_expression.add_code_object(cppentry)

        cpp_macro_inst = macro_instantation.MacroInstantation(cmacro.name, cpp_macro_expression)
        
        return conversion_pack.conversion_validation.validate_macro_instantiation(cmacro, cpp_macro_inst)
