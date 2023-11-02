
from method_conv import *
import constructor_method

# Specialisation of MethodConverter for AccessorData Methods
# From: init(grib_accessor* a, const long len, grib_arguments* arg)
# To:   CONSTRUCTOR(AccessorInitData const& initData)
class ConstructorMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return constructor_method.ConstructorMethod(cppfuncsig)

    def update_cfunction_names(self, line):

        # Transform the argument getters
        line = re.sub(rf"\bgrib_arguments_get_name\s*\(.*?,\s*\w+\s*,\s*(.*)?\)", rf"AccessorName(std::get<std::string>(initData.args[\1].second))", line)
        line = re.sub(rf"\bgrib_arguments_get_(\w+)\(.*?, arg, (\d+)\)", rf"std::get<\1>(initData.args[\2].second)", line)

        return super().update_cfunction_names(line)

    # Overridden to get correct InitData "set size"
    def container_func_call_for(self, cpparg, action, data=""):
        if cpparg.underlying_type == "AccessorInitData":
            if action == "size":
                return "length" # Note - no () as it's a member!
        
        return super().container_funcname_for(cpparg, action)
