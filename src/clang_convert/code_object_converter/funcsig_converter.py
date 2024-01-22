
import utils.debug as debug
import code_object.funcsig as funcsig
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.supporting.funcsig_mapping as funcsig_mapping
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.arg as arg
import code_object.declaration_specifier as declaration_specifier

# Convert a C Function Signature to C++
class FuncSigConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, funcsig.FuncSig), f"Expected FuncSig, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data

        # If we have a mapping already stored, just use that!
        cppfuncsig = self._conversion_data.cppfuncsig_for_cfuncsig(self._ccode_object)
        if not cppfuncsig:
            cppfunc_arg = self.to_cpp_func_arg()
            cpp_args = self.to_cpp_args()
            cppfuncsig = funcsig.FuncSig(cppfunc_arg.decl_spec,
                                         cppfunc_arg.name,
                                         cpp_args,
                                         self._ccode_object.template)

            #cppfuncsig.static = self.is_cpp_static()

            # Add this to the conversion data mappings
            mapping = funcsig_mapping.FuncSigMapping(self._ccode_object, cppfuncsig)
            self._conversion_data.add_funcsig_mapping(mapping)

        # Update the settings that we don't need (want?) to store in the map
        cppfuncsig.is_declaration = self._ccode_object.is_declaration

        return cppfuncsig

    # The following functions are called when there isn't a stored mapping, so we need to work the mappings out!

    def is_cpp_static(self):
        return self._ccode_object.static

    # Converts the name and return type internal representation to C++
    def to_cpp_func_arg(self):
        cfunc_arg = self._ccode_object.func_arg
        cppfunc_arg = conversion_funcs.convert_ccode_object(cfunc_arg, self._conversion_data)
        
        if cppfunc_arg.decl_spec.type == "int":
            # We'll assume int means GribStatus
            cpp_decl_spec = declaration_specifier.DeclSpec(storage_class=cfunc_arg.decl_spec.storage_class,
                                                           const_qualifier=cfunc_arg.decl_spec.const_qualifier,
                                                           type="GribStatus",
                                                           pointer=cfunc_arg.decl_spec.pointer)
            cppfunc_arg = arg.Arg(cpp_decl_spec, cppfunc_arg.name)

        return cppfunc_arg

    # This should return the same number of cppargs as there are cargs (set unused cppargs to None)
    def to_cpp_args(self):
        cppargs = []
        for entry in self._ccode_object.args:
            cpparg = conversion_funcs.convert_ccode_object(entry, self._conversion_data)
            cppargs.append(cpparg)

        return cppargs
      