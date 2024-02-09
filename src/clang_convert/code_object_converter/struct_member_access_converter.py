
import utils.debug as debug
import code_object.struct_member_access as struct_member_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE

class StructMemberAccessConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, struct_member_access.StructMemberAccess), f"Expected StructMemberAccess, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        self._conversion_pack = conversion_pack
        cstruct_member_access = self._ccode_object
        cppstruct_member_access = None
        debug.line("create_cpp_code_object",f"StructMemberAccessConverter [IN] cstruct_member_access=[{debug.as_debug_string(cstruct_member_access)}]")

        # Check if this is a pointer to a class member
        if conversion_pack.conversion_data.is_self_class_pointer_name(cstruct_member_access.name):
            debug.line("create_cpp_code_object",f"StructMemberAccessConverter [1] cstruct_member_access.name=[{debug.as_debug_string(cstruct_member_access.name)}] is a self class pointer name")
            cppstruct_member_access = self.try_to_convert_data_member_access(cstruct_member_access.member)
        
        if not cppstruct_member_access:
            # Check if the primary member is valid...
            cpparg = conversion_pack.conversion_data.cpparg_for_cname(cstruct_member_access.name)
            if cpparg==NONE_VALUE:
                debug.line("create_cpp_code_object", f"StructMemberAccessConverter [2] cstruct_member_access.name=[{cstruct_member_access.name}] cpparg=[{debug.as_debug_string(cpparg)}], attempting to convert member...")
                # See if we can convert the member...
                test_member_access = struct_member_access.StructMemberAccess("", 
                                                                             cstruct_member_access.member.name,
                                                                             cstruct_member_access.member.index,
                                                                             cstruct_member_access.member.member)
                cppstruct_member_access = self.try_to_convert_data_member_access(test_member_access)
            else:
                cpp_access = conversion_funcs.convert_ccode_object(cstruct_member_access.access, conversion_pack)
                cpp_name = conversion_funcs.convert_ccode_object(cstruct_member_access.name, conversion_pack)
                cpp_index = conversion_funcs.convert_ccode_object(cstruct_member_access.index, conversion_pack)
                cpp_member = conversion_funcs.convert_ccode_object(cstruct_member_access.member, conversion_pack)

                cppstruct_member_access = struct_member_access.StructMemberAccess(cpp_access, cpp_name, cpp_index, cpp_member)

        if not cppstruct_member_access:
            return NONE_VALUE

        return conversion_pack.conversion_validation.validate_struct_member_access(cstruct_member_access, cppstruct_member_access)

    # If test member access refers to a data member, convert it!
    def try_to_convert_data_member_access(self, candidate_member_access):
        debug.line("try_to_convert_data_member_access",f"[IN] candidate_member_access=[{debug.as_debug_string(candidate_member_access)}]")
        cppstruct_member_access = None
        cpp_data_member = self._conversion_pack.conversion_data.cppdata_member_for_cdata_member_name(candidate_member_access.name)

        if cpp_data_member:
            cpp_access = ""
            cpp_name = cpp_data_member.name
            cpp_index = conversion_funcs.convert_ccode_object(candidate_member_access.index, self._conversion_pack)
            cpp_member = conversion_funcs.convert_ccode_object(candidate_member_access.member, self._conversion_pack)
            cppstruct_member_access = struct_member_access.StructMemberAccess(cpp_access, cpp_name, cpp_index, cpp_member)

        debug.line("try_to_convert_data_member_access",f"[OUT] cppstruct_member_access=[{debug.as_debug_string(cppstruct_member_access)}]")
        return cppstruct_member_access
