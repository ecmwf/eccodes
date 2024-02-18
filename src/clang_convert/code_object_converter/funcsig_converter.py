
import utils.debug as debug
import code_object.funcsig as funcsig
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_pack.funcsig_mapping as funcsig_mapping
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.arg as arg
import code_object.declaration_specifier as declaration_specifier
from code_object.code_interface import NONE_VALUE

# Convert a C Function Signature to C++
class FuncSigConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, funcsig.FuncSig), f"Expected FuncSig, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cfuncsig = self._ccode_object

        # If we have a mapping already stored, just use that!
        mapping = conversion_pack.conversion_data.funcsig_mapping_for_cfuncname(cfuncsig.name)
        if mapping:
            debug.line("create_cpp_code_object", f"funcsig [1] using existing mapping cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")

            cppfuncsig = mapping.cppfuncsig

            if cppfuncsig == NONE_VALUE:
                debug.line("", f"cppfuncsig is NONE_VALUE for cfuncsig.name=[{cfuncsig.name}] so won't be converted")
                return NONE_VALUE

            cppfuncsig.template_type_params = cfuncsig.template_type_params

            # Add any buffer mappings: {ptr, buffer} -> C++ Container 
            if mapping.arg_indexes:
                cbuffer = cfuncsig.args[mapping.arg_indexes.cbuffer]
                clength = cfuncsig.args[mapping.arg_indexes.clength]
                cpp_container = cppfuncsig.args[mapping.arg_indexes.cpp_container]
                conversion_pack.conversion_data.add_funcsig_buffer_mapping(cbuffer, clength, cpp_container)
        else:
            cppfunc_arg = self.to_cpp_func_arg(conversion_pack)
            cpp_args = self.to_cpp_args(conversion_pack)
            cppfuncsig = funcsig.FuncSig(cppfunc_arg.decl_spec,
                                         cppfunc_arg.name,
                                         cpp_args,
                                         cfuncsig.template_type_params)

            #cppfuncsig.static = self.is_cpp_static()

            # Add this to the correct conversion data mappings
            mapping = funcsig_mapping.FuncSigMapping(cfuncsig, cppfuncsig)

            debug.line("create_cpp_code_object", f"funcsig [2] created new mapping cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")

            is_mem_func = conversion_pack.conversion_data.is_member_function(cfuncsig.name)
            if is_mem_func:
                stored_cppfuncsig = conversion_pack.conversion_data.cppfuncsig_for_cfuncname(cfuncsig.name)
                if not stored_cppfuncsig:
                    conversion_pack.conversion_data.add_global_member_funcsig_mapping(mapping)
            else:
                conversion_pack.conversion_data.add_global_funcsig_mapping(mapping)

        # Add all the arg mappings for the rest of the function to use
        # NOTE: We use cfuncsig rather than mapping.cfuncsig as the former may not have variable names included which will
        #       will cause confusion if multiple args have the same type!
        for i in range(len(mapping.cfuncsig.args)):
            conversion_pack.conversion_data.add_funcsig_arg_mapping(
                cfuncsig.args[i],
                mapping.cppfuncsig.args[i]
            )

        # Update the settings that we don't need (want?) to store in the map
        cppfuncsig.is_declaration = cfuncsig.is_declaration

        return conversion_pack.conversion_validation.validate_funcsig(cfuncsig, cppfuncsig)
    
    # The following functions are called when there isn't a stored mapping, so we need to work the mappings out!

    def is_cpp_static(self):
        return self._ccode_object.static

    # Converts the name and return type internal representation to C++
    def to_cpp_func_arg(self, conversion_pack):
        cfunc_arg = self._ccode_object.func_arg
        cppfunc_arg = conversion_funcs.convert_ccode_object(cfunc_arg, conversion_pack)
        
        if cppfunc_arg.decl_spec.type == "int":
            # We'll assume int means GribStatus
            cpp_decl_spec = declaration_specifier.DeclSpec(storage_class=cfunc_arg.decl_spec.storage_class,
                                                           const_qualifier=cfunc_arg.decl_spec.const_qualifier,
                                                           type="GribStatus",
                                                           pointer=cfunc_arg.decl_spec.pointer)
            cppfunc_arg = arg.Arg(cpp_decl_spec, cppfunc_arg.name)

        return cppfunc_arg

    # This should return the same number of cppargs as there are cargs (set unused cppargs to None)
    def to_cpp_args(self, conversion_pack):
        cppargs = []
        for entry in self._ccode_object.args:
            cpparg = conversion_funcs.convert_ccode_object(entry, conversion_pack)
            cppargs.append(cpparg)

        return cppargs
      