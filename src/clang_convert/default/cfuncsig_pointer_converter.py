
import code_object.funcsig_pointer as funcsig_pointer
import default.cfuncsig_converter as cfuncsig_converter

# Convert a C Function Signature Pointer to C++
#
# Basically the same as the cfuncsig_converter, but uses the funcsig_pointer version of the class

class CFuncSigPointerConverter(cfuncsig_converter.CFuncSigConverter):
    def __init__(self, cfuncsig_pointer) -> None:
        super().__init__(cfuncsig_pointer)

    # Returns both the new cpp funcsig pointer and an updated c funcsig pointer
    def to_cpp_funcsig(self):
        cppfuncsig_pointer = funcsig_pointer.FuncSigPointer(
            self.to_cpp_return_type(), 
            self.to_cpp_name(), 
            self.to_cpp_args())

        cfuncsig_pointer = self._cfuncsig

        return cfuncsig_pointer, cppfuncsig_pointer
