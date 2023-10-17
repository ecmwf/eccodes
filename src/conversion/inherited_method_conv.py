
from method_conv import *
import inherited_method

# Specialisation of MethodConverter for AccessorData Methods
class InheritedMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return inherited_method.InheritedMethod(cppfuncsig)
