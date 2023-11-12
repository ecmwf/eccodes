
from method_conv import *
import constructor_method

# Specialisation of MethodConverter for AccessorData Methods
# From: init(grib_accessor* a, const long len, grib_arguments* arg)
# To:   CONSTRUCTOR(AccessorInitData const& initData)
class ConstructorMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return constructor_method.ConstructorMethod(cppfuncsig, self._transforms.types["self"])

    # overridden to short-circuit well-knwon transforms!
    def convert_cfunction_calls(self, line):

        # Transform the argument getters
        m = re.search(r"((self->)?(\w+)\s+=\s+)?\bgrib_arguments_get_(\w+)\([^,]+, [^,]+, ([^\)]+)\)", line)
        if m:
            ctype = ""
            if m.group(1): 
                member_name = m.group(3)
                for cmember, cppmember in self._transforms.members.items():
                    if cmember.name == member_name:
                        ctype = cppmember.type
                        break

            get_what = m.group(4)
            if get_what == "expression":
                get_what = "GribExpressionPtr"
            elif get_what in ["string", "name"]:
                get_what = "std::string"

            if ctype == "AccessorName":
                line = re.sub(m.re, f"{m.group(1)}AccessorName(std::get<{get_what}>(initData.args[{m.group(5)}].second))", line)
            else:
                line = re.sub(m.re, f"{m.group(1)}std::get<{get_what}>(initData.args[{m.group(5)}].second)", line)
            
            debug.line("convert_cfunction_calls", f"Updated [bgrib_arguments_get_expression] line=[{line}]")

        return super().convert_cfunction_calls(line)

    # Overridden to get correct InitData "set size"
    def container_func_call_for(self, cpparg, action, data=""):
        if cpparg.underlying_type == "AccessorInitData":
            if action == "size":
                return "length" # Note - no () as it's a member!
        
        return super().container_func_call_for(cpparg, action)
