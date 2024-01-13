import code_object.code_interface as code_interface
import copy
import debug

# Base class for converting a C code object to C++
#
# Call convert_ccode_object() for generic convert behaviour
class CodeInterfaceConverter:
    def __init__(self, ccode_object) -> None:
        assert isinstance(ccode_object, code_interface.CodeInterface), f"Cannot convert object of type [{type(ccode_object)}]"
        self._ccode_object = ccode_object
    
    # Convert the ccode_object to a cpp_code_object
    # It must be overridden - this version just returns a copy of the ccode_object
    def to_cpp_code_object(self, conversion_data):
        cpp_code_object = copy.deepcopy(self._ccode_object)
        debug.line("to_cpp_code", f"Base version - just returning a copy of the C code object")
        return cpp_code_object
