
import code_object.funcsig_pointer as funcsig_pointer
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.funcsig_pointer_mapping as funcsig_pointer_mapping

# Convert a C Function Signature Pointer to C++
#
# Basically the same as the funcsig_converter, but uses the funcsig_pointer version of the class

class FuncSigPointerConverter(funcsig_converter.FuncSigConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, funcsig_pointer.FuncSigPointer), f"Expected FuncSigPointer, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data

        # If we have a mapping already stored, just use that!
        cppfuncsig_pointer = self._conversion_data.cppfuncsig_pointer_for_cfuncsig_pointer(self._ccode_object)
        if not cppfuncsig_pointer:
            cppfuncsig_pointer = funcsig_pointer.FuncSigPointer(
                self.to_cpp_return_type(), 
                self.to_cpp_name(), 
                self.to_cpp_args())

            #cppfuncsig.static = self.is_cpp_static()

            # Add this to the conversion data mappings
            mapping = funcsig_pointer_mapping.FuncSigPointerMapping(self._ccode_object, cppfuncsig_pointer)
            self._conversion_data.add_to_funcsig_pointer_mappings(mapping)

        # Update the settings that we don't need (want?) to store in the map
        cppfuncsig_pointer.is_declaration = self._ccode_object.is_declaration

        return cppfuncsig_pointer