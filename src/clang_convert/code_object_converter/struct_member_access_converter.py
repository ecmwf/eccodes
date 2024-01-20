
import utils.debug as debug
import code_object.struct_member_access as struct_member_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class StructMemberAccessConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, struct_member_access.StructMemberAccess), f"Expected StructMemberAccess, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):

        cpp_access = conversion_funcs.convert_ccode_object(self._ccode_object.access, conversion_data)
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_data)
        cpp_index = conversion_funcs.convert_ccode_object(self._ccode_object.index, conversion_data)
        cpp_member = conversion_funcs.convert_ccode_object(self._ccode_object.member, conversion_data)

        return struct_member_access.StructMemberAccess(cpp_access, cpp_name, cpp_index, cpp_member)