
import utils.debug as debug
from code_object.code_interface import CodeInterface
from code_object.struct_member_access import StructMemberAccess
from code_object.literal import Literal
import code_object_converter.conversion_pack.buffer_mapping as buffer_mapping
import code_object_converter.conversion_pack.arg_utils as arg_utils
from code_object.code_interface import NONE_VALUE

class ContainerUtils:
    def create_cpp_container_buffer_arg(self, name):
        cpp_member = StructMemberAccess(None, name, None)
        debug.line("create_cpp_container_buffer_arg", f"cpp_member=[{debug.as_debug_string(cpp_member)}]")
        return cpp_member

    def create_cpp_container_length_arg(self, name):
        cpp_member = self.create_cpp_container_buffer_arg(name)
        cpp_member.member = StructMemberAccess(".", "size()", None)
        debug.line("create_cpp_container_length_arg", f"cpp_member=[{debug.as_debug_string(cpp_member)}]")
        return cpp_member

    # If cbuffer matches the cbuffer in a buffer map, return the buffer map, else None
    def cbuffer_to_buffer_mapping(self, cbuffer, conversion_data):
        buf_map = conversion_data.funcsig_buffer_mapping_for_cname(cbuffer)
        if buf_map and buf_map.cbuffer.name == cbuffer:
            return buf_map

        return None

    # If clength matches the clength in a buffer map, return the buffer map, else None
    def clength_to_buffer_mapping(self, clength, conversion_data):
        buf_map = conversion_data.funcsig_buffer_mapping_for_cname(clength)
        if buf_map and buf_map.clength.name == clength:
            return buf_map

        return None

    # Tries to determine if the supplied cname maps to a C++ container type.
    # If a match is found, returns a StructMemberAccess representing "cpp_container", else None
    def cname_to_cpp_container_buffer(self, cname, conversion_data):

        # 1. Check buffer maps
        buf_map = self.cbuffer_to_buffer_mapping(cname, conversion_data)
        if buf_map:
            return self.create_cpp_container_buffer_arg(buf_map.cpp_container.name)

        # 2. Try standard arg conversions
        cpparg = conversion_data.funcbody_cpparg_for_carg_name(cname)
        if cpparg and cpparg != NONE_VALUE and conversion_data.is_container_type(cpparg.decl_spec.type):
            return self.create_cpp_container_buffer_arg(cpparg.name)

        return None

    # Tries to determine if the supplied cname maps to a C++ container type.
    # If a match is found, returns a StructMemberAccess representing "cpp_container.size()", else None
    def cname_to_cpp_container_length(self, cname, conversion_data):

        # 1. Check buffer maps
        buf_map = self.clength_to_buffer_mapping(cname, conversion_data)
        if buf_map:
            return self.create_cpp_container_length_arg(buf_map.cpp_container.name)

        # 2. Try standard arg conversions
        cpparg = conversion_data.funcbody_cpparg_for_carg_name(cname)
        if cpparg and conversion_data.is_container_type(cpparg.decl_spec.type):
            return self.create_cpp_container_length_arg(cpparg.name)

        return None

    # Use this version when you need to check if cname is either buf or len in {buf, len}
    # and return the appropriate C++ container value
    def cname_to_cpp_container(self, cname, conversion_data):

        buf_map = self.clength_to_buffer_mapping(cname, conversion_data)
        if buf_map:
            return self.create_cpp_container_length_arg(buf_map.cpp_container.name)
        
        return self.cname_to_cpp_container_buffer(cname, conversion_data)