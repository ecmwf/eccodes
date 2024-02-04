
import utils.debug as debug
from code_object.struct_member_access import StructMemberAccess
import default.default_conversion_pack.default_container_utils as default_container_utils

class GribAccessorContainerUtils(default_container_utils.DefaultContainerUtils):

    def create_cpp_container_length_arg(self, name):
        cpp_member = super().create_cpp_container_length_arg(name)

        if cpp_member.name == "initData":
            # Need to change from initData.size() to initData.args.size()
            args_member = StructMemberAccess(".", "args", None)
            size_member = StructMemberAccess(".", "size()", None)
            args_member.member = size_member
            cpp_member.member = args_member

            debug.line("create_cpp_container_length_arg", f"Updated initData length access: cpp_member=[{debug.as_debug_string(cpp_member)}]")

        return cpp_member

