
import utils.debug as debug
import code_object.struct_member_access as struct_member_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class StructMemberAccessConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, struct_member_access.StructMemberAccess), f"Expected StructMemberAccess, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        debug.line("create_cpp_code_object",f" StructMemberAccessConverter [IN] self._ccode_object=[{debug.as_debug_string(self._ccode_object)}]")

        # Check if this is a pointer to a class instance
        if conversion_pack.conversion_data.is_class_pointer_name(self._ccode_object.name):
            cstruct_member_access = self._ccode_object.member
            cpp_access = ""
            cpp_data_member = conversion_pack.conversion_data.cppdata_member_for_cdata_member_name(cstruct_member_access.name)
            assert cpp_data_member
            cpp_name = cpp_data_member.name
            cpp_index = conversion_funcs.convert_ccode_object(cstruct_member_access.index, conversion_pack)
            cpp_member = conversion_funcs.convert_ccode_object(cstruct_member_access.member, conversion_pack)
        else:
            cstruct_member_access = self._ccode_object
            cpp_access = conversion_funcs.convert_ccode_object(cstruct_member_access.access, conversion_pack)
            cpp_name = conversion_funcs.convert_ccode_object(cstruct_member_access.name, conversion_pack)
            cpp_index = conversion_funcs.convert_ccode_object(cstruct_member_access.index, conversion_pack)
            cpp_member = conversion_funcs.convert_ccode_object(cstruct_member_access.member, conversion_pack)

        cppstruct_member_access = struct_member_access.StructMemberAccess(cpp_access, cpp_name, cpp_index, cpp_member)
        return conversion_pack.conversion_validation.validate_struct_member_access(self._ccode_object, cppstruct_member_access)
