
from method_conv import *
import destructor_method

# Specialisation of MethodConverter for AccessorData Methods
class DestructorMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return destructor_method.DestructorMethod(cppfuncsig)
