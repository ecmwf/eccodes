
import utils.debug as debug
import code_object.declaration_specifier as declaration_specifier
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class DeclSpecConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, declaration_specifier.DeclSpec), f"Expected DeclSpec, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cdecl_spec = self._ccode_object
        cpp_decl_spec, _ = conversion_data.closest_cppdecl_spec_for_cdecl_spec(cdecl_spec)

        if not cpp_decl_spec:
            cpp_decl_spec = declaration_specifier.DeclSpec(storage_class=cdecl_spec.storage_class, 
                                                           const_qualifier=cdecl_spec.const_qualifier, 
                                                           type=cdecl_spec.type, 
                                                           pointer=cdecl_spec.pointer)

            conversion_data.add_type_mapping(cdecl_spec, cpp_decl_spec)
            debug.line("create_cpp_code_object", f"DeclSpec conversion: [{cdecl_spec.as_string()}] -> [{cpp_decl_spec.as_string()}]")

        return cpp_decl_spec
  