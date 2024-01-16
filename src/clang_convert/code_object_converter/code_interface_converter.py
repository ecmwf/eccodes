import code_object.code_interface as code_interface
import copy
import utils.debug as debug

# Base class for converting a C code object to C++
#
# Call convert_ccode_object() for generic convert behaviour
class CodeInterfaceConverter:
    def __init__(self, ccode_object) -> None:
        assert isinstance(ccode_object, code_interface.CodeInterface), f"Cannot convert object of type [{type(ccode_object)}]"
        self._ccode_object = ccode_object
    
    # Convert the ccode_object to a cpp_code_object
    #
    # This is the function that *MUST* be called externally, as it ensures a copy
    # of the converted object is returned to avoid reference issues (funcsig is particularly
    # prone to this!)
    # 
    # The real work is done in create_cpp_code_object() !!!
    def to_cpp_code_object(self, conversion_data):
        cpp_code_object = self.create_cpp_code_object(conversion_data)
        return copy.deepcopy(cpp_code_object)

    # Actual implementation of to_cpp_code_object()
    # It must be overridden - this version just returns the passed in ccode_object!
    def create_cpp_code_object(self, conversion_data):
        debug.line("to_cpp_code_object_internal", f"Base version - just returning the C code object")
        return self._ccode_object
