
import utils.debug as debug
import code_object.array_access as array_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class ArrayAccessConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, array_access.ArrayAccess), f"Expected ArrayAccess, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_pack)
        cpp_index = conversion_funcs.convert_ccode_object(self._ccode_object.index, conversion_pack)

        cpparray_access = array_access.ArrayAccess(cpp_name, cpp_index)
        return conversion_pack.conversion_validation.validate_array_access(self._ccode_object, cpparray_access)
