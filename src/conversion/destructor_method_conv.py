
from method_conv import *
import destructor_method

# Specialisation of MethodConverter for AccessorData Methods
class DestructorMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return destructor_method.DestructorMethod(cppfuncsig, self._transforms.types["self"])

    # Overridden to create an empty constructor if name is None
    def create_cpp_body(self):
        if self._cppfunction.name is None:
            debug.line("create_cpp_body", f"\n============================== {self._cfunction.name} [IN]  ==============================\n")
            debug.line("create_cpp_body", f"Creating empty destructor body as no conversion defined (name is None)")
            debug.line("create_cpp_body", f"\n============================== {self._cfunction.name} [OUT] ==============================\n")
            return ["/* Destructor has been deleted */"]

        return super().create_cpp_body()
