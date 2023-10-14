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
    def to_cpp_arg(self, type_transforms=arg_conv.common_type_transforms):
        cppmember = None

        # If then type is grib_accessor_ pointer, then we'll convert to an AccessorName
        if re.search(r"grib_accessor\w*\*", self._carg.type):
            cppname = arg_conv.transform_variable_name(self._carg.name) + "_"
            cppmember = member.Member("AccessorName", cppname)
            cppmember.default_value = "{\"\"}"
        else:
            cpp_arg = super().to_cpp_arg(type_transforms)
            if cpp_arg:
                cppmember = member.Member(cpp_arg.type, cpp_arg.name + "_")

                # We'll assume "const char*" means this variable refers to another accessor...
                if self._carg.type == "const char*":
                    cppmember.type = "AccessorName"
                    cppmember.default_value = "{\"\"}"
                else:
                    cppmember.default_value = ""

                cppmember._mutable = False
        
        return cppmember

    def to_cpp_func_sig_arg(self, type_transforms = arg_conv.common_type_transforms):
        assert False, "to_cpp_func_sig_arg not supported for members"