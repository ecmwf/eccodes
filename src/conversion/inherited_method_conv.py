
from method_conv import *
import inherited_method

# Specialisation of MethodConverter for AccessorData Methods
class InheritedMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return inherited_method.InheritedMethod(cppfuncsig)

    # Comment out inherited methods that we aren't ready to implement yet...
    def create_commented_cpp_body(self):
        debug.line("create_commented_cpp_body", f"\n============================== {self._cfunction.name} [IN]  ==============================\n")
        
        cpp_lines = []
        cpp_lines.append("// C++ implementation not yet available.")
        cpp_lines.append("// Commented C body provided below for reference:\n")
        
        for line in self._cfunction.code:
            cpp_lines.append("// " + line)
        cpp_lines.append("")
        
        cpp_lines.append(f"return {self._transforms.types['super']}::{self._cppfunction.name}({self._cppfunction.parent_call_arg_string});")
        
        debug.line("create_commented_cpp_body", f"\n============================== {self._cfunction.name} [OUT] ==============================\n")

        return cpp_lines


    # Overridden to handle specific cases
    def create_cpp_body(self):
        if self._cppfunction.name in ["dump", "compare"]:
            return self.create_commented_cpp_body()

        return super().create_cpp_body()
