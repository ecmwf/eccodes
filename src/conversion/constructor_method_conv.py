
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

    # Overridden to get correct InitData "set size"
    def container_func_call_for(self, cpparg, action, data=""):
        if cpparg.underlying_type == "AccessorInitData":
            if action == "size":
                return "length" # Note - no () as it's a member!
        
        return super().container_func_call_for(cpparg, action)
