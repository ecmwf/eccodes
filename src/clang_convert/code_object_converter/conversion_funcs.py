import code_object.arg as arg
import code_object.compound_statement as compound_statement
import code_object.code_objects as code_objects
import code_object.funcsig as funcsig

import code_object_converter.arg_converter as arg_converter
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.compound_statement_converter as compound_statement_converter

import utils.debug as debug

# Mapping from C code objects to their converters
CodeInterfaceConverterClasses = {
    arg.Arg                                 : arg_converter.ArgConverter,
    compound_statement.CompoundStatement    : compound_statement_converter.CompoundStatementConverter,
    funcsig.FuncSig                         : funcsig_converter.FuncSigConverter,
}

# Convert a single C code_object into a C++ code_object
def convert_ccode_object(ccode_object, conversion_data):
    converter_class = CodeInterfaceConverterClasses.get(type(ccode_object), code_interface_converter.CodeInterfaceConverter)
    debug.line("convert_ccode_object", f"ccode_object type=[{type(ccode_object)}] converter_class=[{type(converter_class)}]")
    converter = converter_class(ccode_object)
    cpp_obj = converter.to_cpp_code_object(conversion_data)

    return cpp_obj

# Convert a collection of C code_objects into C++ code_objects
def convert_ccode_object_collection(ccode_objects, conversion_data):
    cpp_code_objects = code_objects.CodeObjects()
    for cobj in ccode_objects.code_objects:
        debug.line("convert_ccode_object_collection",f"cobj as string=[{cobj.as_string()}]")
        cpp_obj = convert_ccode_object(cobj, conversion_data)
        cpp_code_objects.add_code_object(cpp_obj)

    return cpp_code_objects

