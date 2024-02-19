
import utils.debug as debug
import code_object.array_access as array_access
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.struct_member_access as struct_member_access
from code_object.code_interface import NONE_VALUE
import code_object_converter.conversion_pack.arg_utils as arg_utils

class ArrayAccessConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, array_access.ArrayAccess), f"Expected ArrayAccess, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_pack)
        cpp_index = conversion_funcs.convert_ccode_object(self._ccode_object.index, conversion_pack)

        if cpp_name == NONE_VALUE:
            debug.line("create_cpp_code_object", f"cpp_name=[{debug.as_debug_string(cpp_name)}] cpp_index=[{debug.as_debug_string(cpp_index)}] for self._ccode_object=[{debug.as_debug_string(self._ccode_object)}]  => returning NONE_VALUE")
            return NONE_VALUE

        # If the name has been converted to a struct_member_access, representing a container size, e.g. foo.size(), 
        # then we need to deal with the array access part!
        if isinstance(cpp_name, struct_member_access.StructMemberAccess):
             
             if cpp_name.member:
                cpp_member_name = arg_utils.extract_name(cpp_name.member)
                if cpp_member_name and cpp_member_name == "size()":
                    # If we're accessing index 0, then we can just remove the [0] part,
                    # otherwise something unexpected has happened!
                    if cpp_index.as_string() == "0":
                        debug.line("create_cpp_code_object", f"Stripping array access, from=[{debug.as_debug_string(self._ccode_object)}] to=[{debug.as_debug_string(cpp_name)}]")
                        return cpp_name
                    else:
                        assert False, f"Unexpected array access=[{debug.as_debug_string(self._ccode_object)}]"

        cpparray_access = array_access.ArrayAccess(cpp_name, cpp_index)

        return conversion_pack.conversion_validation.validate_array_access(self._ccode_object, cpparray_access)
