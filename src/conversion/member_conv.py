# Convert Member from C to C++

import member
import arg
import arg_conv
import debug
import re

class MemberConverter(arg_conv.ArgConverter):
    def __init__(self, initial_carg):
        super().__init__(initial_carg)

    # Override for extra changes required for a member, such as ending with "_"
    # Need to ensure we return a member, not an arg!
    def to_cpp_arg(self, transforms):
        cppmember = None

        # Check if we have a custom member setting first...
        if self._initial_carg in transforms._custom_member_args:
            cpp_arg = transforms._custom_member_args[self._initial_carg]
            cppmember = member.Member(cpp_arg.type, cpp_arg.name + "_")
            cppmember.default_value = ""
            cppmember._mutable = False
            debug.line("to_cpp_arg", f"[Member] Using custom member transform: [{arg.arg_string(self._initial_carg)}] -> [{arg.arg_string(cppmember)}]")
            return cppmember

        # We'll assume "const char*" and "grib_accessor*" types mean this variable refers to another accessor...
        if self._initial_carg.type in ["const char*", "grib_accessor*"]:
            cppmember_name = arg_conv.transform_variable_name(self._initial_carg.name) + "_"
            cppmember = member.Member("AccessorName", cppmember_name)
            cppmember.default_value = "{\"\"}"
            cppmember._mutable = False
        else:
            cpp_arg = super().to_cpp_arg(transforms)
            if cpp_arg:
                cppmember = member.Member(cpp_arg.type, cpp_arg.name + "_")
                cppmember.default_value = ""
                cppmember._mutable = False

        return cppmember

