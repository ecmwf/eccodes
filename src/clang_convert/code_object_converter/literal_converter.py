
import utils.debug as debug
import code_object.literal as literal
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class LiteralConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, literal.Literal), f"Expected Literal, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):

        # 1. Check for a literal mapping
        cppliteral = conversion_data.cppstring_literal_for_cstring(self._ccode_object.value)
        if cppliteral:
            return literal.Literal(cppliteral)

        # 2. Default is to just return a copy
        return literal.Literal(self._ccode_object.value)
