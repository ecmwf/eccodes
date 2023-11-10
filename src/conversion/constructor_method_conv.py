
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
        line, count = re.subn(rf"\bgrib_arguments_get_name\s*\(.*?,\s*\w+\s*,\s*([^\)]*)?\)", rf"AccessorName(std::get<std::string>(initData.args[\1].second))", line)
        if count:
            debug.line("convert_cfunction_calls", f"Updated [grib_arguments_get_name] line=[{line}]")
            #return line

        line, count = re.subn(rf"\bgrib_arguments_get_expression\s*\(.*?,\s*\w+\s*,\s*([^\)]*)?\)", rf"std::get<GribExpressionPtr>(initData.args[\1].second)", line)
        if count:
            debug.line("convert_cfunction_calls", f"Updated [bgrib_arguments_get_expression] line=[{line}]")
            #return line

        line, count = re.subn(rf"\bgrib_arguments_get_(\w+)\([^,]+, [^,]+, ([^\)]+)\)", rf"std::get<\1>(initData.args[\2].second)", line)
        if count:
            debug.line("convert_cfunction_calls", f"Updated [grib_arguments_get_X] line=[{line}]")
            #return line

        return super().convert_cfunction_calls(line)

    # Overridden to get correct InitData "set size"
    def container_func_call_for(self, cpparg, action, data=""):
        if cpparg.underlying_type == "AccessorInitData":
            if action == "size":
                return "length" # Note - no () as it's a member!
        
        return super().container_func_call_for(cpparg, action)
