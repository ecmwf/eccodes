
import utils.debug as debug
import code_object.struct_arg as struct_arg
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.declaration_specifier as declaration_specifier

class StructArgConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, struct_arg.StructArg), f"Expected StructArg, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        # Note: The struct name actually defines a type...
        cpp_decl_spec, _ = conversion_data.closest_cppdecl_spec_for_ctype(self._ccode_object.name)

        if not cpp_decl_spec:
            cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_data)
            cdecl_spec = declaration_specifier.DeclSpec(type=self._ccode_object.name, pointer="")
            cpp_decl_spec = declaration_specifier.DeclSpec(type=cpp_name, pointer="")
            conversion_data.add_type_mapping(cdecl_spec, cpp_decl_spec)
            debug.line("create_cpp_code_object", f"StructArg type mapping: [{cdecl_spec.as_string()}] -> [{cpp_decl_spec.as_string()}]")

        cpp_members=[]
        for member in self._ccode_object.members:
            cpp_member = conversion_funcs.convert_ccode_object(member, conversion_data)
            cpp_members.append(cpp_member)

        return struct_arg.StructArg(cpp_decl_spec.type, cpp_members)
