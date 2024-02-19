
import utils.debug as debug
from code_object.struct_member_access import StructMemberAccess
import default.default_conversion_pack.default_container_utils as default_container_utils
import code_object_converter.conversion_pack.arg_utils as arg_utils
import code_object.literal as literal


class GribAccessorContainerUtils(default_container_utils.DefaultContainerUtils):

    def create_cpp_container_length_arg(self, name):
        cpp_member = super().create_cpp_container_length_arg(name)

        cpp_name = arg_utils.extract_name(cpp_member)
        if cpp_name and cpp_name == "initData":
            # Need to change from initData.size() to initData.length
            args_member = StructMemberAccess(".", literal.Literal("length"), None)
            cpp_member.member = args_member

            debug.line("create_cpp_container_length_arg", f"Updated initData length access: cpp_member=[{debug.as_debug_string(cpp_member)}]")

        return cpp_member

