
import code_object.funcsig_pointer as funcsig_pointer
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.supporting.funcsig_pointer_mapping as funcsig_pointer_mapping
import code_object.declaration_specifier as declaration_specifier
import utils.debug as debug

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
            cppfunc_arg = self.to_cpp_func_arg()
            cpp_args = self.to_cpp_args()

            cppfuncsig_pointer = funcsig_pointer.FuncSigPointer(cppfunc_arg.decl_spec,
                                                                cppfunc_arg.name,
                                                                cpp_args)

            #cppfuncsig.static = self.is_cpp_static()

            # Add this to the conversion data mappings
            mapping = funcsig_pointer_mapping.FuncSigPointerMapping(self._ccode_object, cppfuncsig_pointer)
            self._conversion_data.add_funcsig_pointer_mapping(mapping)

            # We've also created a new type, so need to add this too!
            cdecl_spec = declaration_specifier.DeclSpec(type=self._ccode_object.func_arg.name, pointer=self._ccode_object.func_arg.decl_spec.pointer)
            cppdecl_spec = declaration_specifier.DeclSpec(type=cppfunc_arg.name, pointer=cppfunc_arg.decl_spec.pointer)
            self._conversion_data.add_type_mapping(cdecl_spec, cppdecl_spec)
            debug.line("create_cpp_code_object", f"FuncSigPointer conversion: [{cdecl_spec.as_string()}] -> [{cppdecl_spec.as_string()}]")


        # Update the settings that we don't need (want?) to store in the map
        cppfuncsig_pointer.is_declaration = self._ccode_object.is_declaration

        return cppfuncsig_pointer