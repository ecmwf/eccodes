# Convert Member from C to C++

import member
import arg_conv
import debug
import re

class MemberConverter(arg_conv.ArgConverter):
    def __init__(self, carg):
        super().__init__(carg)

    # Override for extra changes required for a member, such as ending with "_"
    # Need to ensure we return a member, not an arg!
    def to_cpp_arg(self, transforms):
        cppmember = None

        # We'll assume "const char*" and "grib_accessor*" types mean this variable refers to another accessor...
        if self._carg.type in ["const char*", "grib_accessor*"]:
            cppmember_name = arg_conv.transform_variable_name(self._carg.name) + "_"
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

    def to_cpp_func_sig_arg(self, transforms):
        assert False, "to_cpp_func_sig_arg not supported for members"
