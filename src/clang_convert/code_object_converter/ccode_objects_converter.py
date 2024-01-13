import code_object_converter.code_interface_converter as code_interface_converter
import code_object.code_objects as code_objects
import debug

# Mapping from C code objects to their converters
CodeInterfaceConverterClasses = {
#    code_interface.CodeInterface: CodeInterfaceConverter,
}

# Convert a collection of C code_objects into C++ code_objects
def convert_ccode_objects(ccode_objects, conversion_data):
    cpp_code_objects = code_objects.CodeObjects()
    for cobj in ccode_objects.code_objects:
        converter_class = CodeInterfaceConverterClasses.get(type(cobj), code_interface_converter.CodeInterfaceConverter)
        debug.line("convert_ccode_object", f"ccode_object type=[{type(cobj)}] converter_class=[{type(converter_class)}]")
        converter = converter_class(cobj)
        cpp_obj = converter.to_cpp_code_object(conversion_data)
        cpp_code_objects.add_code_object(cpp_obj)

    return cpp_code_objects
