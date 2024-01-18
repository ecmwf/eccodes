
import utils.debug as debug
import code_object.declaration_specifier as declaration_specifier
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class DeclSpecConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, declaration_specifier.DeclSpec), f"Expected DeclSpec, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_storage_class = self._ccode_object.storage_class
        cpp_const_qualifier = self._ccode_object.const_qualifier
        cpp_type = self._ccode_object.type
        cpp_pointer = self._ccode_object.pointer

        return declaration_specifier.DeclSpec(storage_class=cpp_storage_class, 
                                              const_qualifier=cpp_const_qualifier, 
                                              type=cpp_type, 
                                              pointer=cpp_pointer)
  