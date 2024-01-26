
import utils.debug as debug
import code_object.value_declaration_reference as value_declaration_reference
import code_object.arg as arg
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
from code_object.code_interface import NONE_VALUE

class ValueDeclarationReferenceConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, value_declaration_reference.ValueDeclarationReference), f"Expected ValueDeclarationReference, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cdecl_ref_expr_value = self._ccode_object.as_string()

        # 1. Check if it is a function name
        cppfuncsig = conversion_pack.conversion_data.cppfuncsig_for_cfuncname(cdecl_ref_expr_value)
        debug.line("create_cpp_code_object", f"ValueDeclarationReferenceConverter [1] cdecl_ref_expr_value=[{debug.as_debug_string(cdecl_ref_expr_value)}] cppfuncsig=[{debug.as_debug_string(cppfuncsig)}]")
        if cppfuncsig:
            return value_declaration_reference.ValueDeclarationReference(cppfuncsig.name)
        
        # 2. Check if it is an arg
        cpparg = conversion_pack.conversion_data.funcbody_cpparg_for_carg_name(cdecl_ref_expr_value)
        debug.line("create_cpp_code_object", f"ValueDeclarationReferenceConverter [2] cdecl_ref_expr_value=[{debug.as_debug_string(cdecl_ref_expr_value)}] cpparg=[{debug.as_debug_string(cpparg)}]")
        if cpparg == NONE_VALUE:
            return NONE_VALUE
        elif cpparg:
            return value_declaration_reference.ValueDeclarationReference(cpparg.name)

        # 3. Check if it is a mapped buffer (e.g. char *c, int* len => std::string& str)
        

        # 4. Perform a default conversion
        cppdecl_ref_expr_value = conversion_funcs.convert_ccode_object(cdecl_ref_expr_value, conversion_pack)
        debug.line("create_cpp_code_object", f"ValueDeclarationReferenceConverter [4] cdecl_ref_expr_value=[{debug.as_debug_string(cdecl_ref_expr_value)}] cppdecl_ref_expr_value=[{debug.as_debug_string(cppdecl_ref_expr_value)}]")

        return value_declaration_reference.ValueDeclarationReference(cppdecl_ref_expr_value)
