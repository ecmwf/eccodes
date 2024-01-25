import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.code_objects as code_objects
import utils.debug as debug

# Base class for converting a C code object to C++
#
# Call convert_ccode_object() for generic convert behaviour
class CodeObjectsConverter(code_interface_converter.CodeInterfaceConverter):
    
    # Override to set the correct class (e.g. Compound Statement)
    @property
    def code_objects_class(self):
        return code_objects.CodeObjects

    def create_cpp_code_object(self, conversion_pack):
        cpp_code_objects = self.code_objects_class()

        for entry in self._ccode_object.code_objects:
            converted_object = conversion_funcs.convert_ccode_object(entry, conversion_pack)
            cpp_code_objects.add_code_object(converted_object)

        return cpp_code_objects
