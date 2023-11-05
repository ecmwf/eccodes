
from method_conv import *
import private_method

# Specialisation of MethodConverter for AccessorData Methods
class PrivateMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return private_method.PrivateMethod(cppfuncsig, self._transforms.types["self"])
