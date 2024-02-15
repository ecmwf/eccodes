
import utils.debug as debug
import code_object.struct_member_access as struct_member_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE
import code_object_converter.conversion_pack.arg_utils as arg_utils
import code_object.literal as literal

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
        variable_name = arg_utils.extract_name(cstruct_member_access.variable)
        if conversion_pack.conversion_data.is_self_class_pointer_name(variable_name):
            debug.line("create_cpp_code_object",f"StructMemberAccessConverter [1] cstruct_member_access.variable=[{debug.as_debug_string(cstruct_member_access.variable)}] is a self class pointer variable")
            cppstruct_member_access = self.try_to_convert_data_member_access(cstruct_member_access.member)
        
        if not cppstruct_member_access:
            # Check if the primary member is valid...
            cpparg = conversion_pack.conversion_data.cpparg_for_cname(variable_name)
            if cpparg==NONE_VALUE:
                debug.line("create_cpp_code_object", f"StructMemberAccessConverter [2] cstruct_member_access.variable=[{cstruct_member_access.variable}] cpparg=[{debug.as_debug_string(cpparg)}] cstruct_member_access.member=[{debug.as_debug_string(cstruct_member_access.member)}]")
                if cstruct_member_access.member:
                    # See if we can convert the member...
                    debug.line("create_cpp_code_object", f"StructMemberAccessConverter [2.1] Attempting to convert cstruct_member_access.member=[{debug.as_debug_string(cstruct_member_access.member)}]...")
                    test_member_access = struct_member_access.StructMemberAccess("", 
                                                                                cstruct_member_access.member.variable,
                                                                                cstruct_member_access.member.member)
                    cppstruct_member_access = self.try_to_convert_data_member_access(test_member_access)
            else:
                cpp_access = conversion_funcs.convert_ccode_object(cstruct_member_access.access, conversion_pack)
                cpp_variable = conversion_funcs.convert_ccode_object(cstruct_member_access.variable, conversion_pack)
                cpp_member = conversion_funcs.convert_ccode_object(cstruct_member_access.member, conversion_pack)

                cppstruct_member_access = struct_member_access.StructMemberAccess(cpp_access, cpp_variable, cpp_member)

        if not cppstruct_member_access:
            return NONE_VALUE

        return conversion_pack.conversion_validation.validate_struct_member_access(cstruct_member_access, cppstruct_member_access)

    # If test member access refers to a data member, convert it!
    def try_to_convert_data_member_access(self, candidate_member_access):
        debug.line("try_to_convert_data_member_access",f"[IN] candidate_member_access=[{debug.as_debug_string(candidate_member_access)}]")
        cppstruct_member_access = None
        search_variable = arg_utils.extract_name(candidate_member_access.variable)
        debug.line("try_to_convert_data_member_access", f"Searching [1] for data member for variable=[{search_variable}]")
        cpp_data_member = self._conversion_pack.conversion_data.cppdata_member_for_cdata_member_name(search_variable)

        if not cpp_data_member:
            search_variable = candidate_member_access.as_string()
            debug.line("try_to_convert_data_member_access", f"Searching [2] for data member for variable=[{search_variable}]")
            cpp_data_member = self._conversion_pack.conversion_data.cppdata_member_for_cdata_member_name(search_variable)

        if cpp_data_member:
            cpp_access = ""
            cpp_variable = literal.Literal(cpp_data_member.name)
            cpp_member = conversion_funcs.convert_ccode_object(candidate_member_access.member, self._conversion_pack)
            cppstruct_member_access = struct_member_access.StructMemberAccess(cpp_access, cpp_variable, cpp_member)

        debug.line("try_to_convert_data_member_access",f"[OUT] cppstruct_member_access=[{debug.as_debug_string(cppstruct_member_access)}]")
        return cppstruct_member_access
