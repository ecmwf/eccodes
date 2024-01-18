import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.compound_statement as compound_statement
import utils.debug as debug

# Base class for converting a C code object to C++
#
# Call convert_ccode_object() for generic convert behaviour
class CompoundStatementConverter(code_interface_converter.CodeInterfaceConverter):
    
    def create_cpp_code_object(self, conversion_data):
        debug.line("create_cpp_code_object", f"Creating C++ compound statement")
        cpp_compound_statement = compound_statement.CompoundStatement()
        converted_objects = conversion_funcs.convert_ccode_object_collection(self._ccode_object, conversion_data)
        cpp_compound_statement.add_code_object_collection(converted_objects)
        return cpp_compound_statement
