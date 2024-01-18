
import code_object.arg as arg
import code_object.code_objects as code_objects
import code_object.compound_statement as compound_statement
import code_object.funcsig as funcsig
import code_object.variable_declaration as variable_declaration

import code_object_converter.arg_converter as arg_converter
import code_object_converter.code_objects_converter as code_objects_converter
import code_object_converter.compound_statement_converter as compound_statement_converter
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.variable_declaration_converter as variable_declaration_converter

import code_object_converter.code_interface_converter as code_interface_converter
import utils.debug as debug

# Mapping from C code objects to their converters
CodeInterfaceConverterClasses = {
    arg.Arg                                     : arg_converter.ArgConverter,
    code_objects.CodeObjects                    : code_objects_converter.CodeObjectsConverter,
    compound_statement.CompoundStatement        : compound_statement_converter.CompoundStatementConverter,
    funcsig.FuncSig                             : funcsig_converter.FuncSigConverter,
    variable_declaration.VariableDeclaration    : variable_declaration_converter.VariableDeclarationConverter,
}

# Convert a code_object into a C++ code_object
def convert_ccode_object(ccode_object, conversion_data):
    debug.line("convert_ccode_object", f"Converting ccode_object instance=[{ccode_object}]")

    converter_class = CodeInterfaceConverterClasses.get(type(ccode_object), code_interface_converter.CodeInterfaceConverter)
    debug.line("convert_ccode_object", f"converter_class=[{converter_class}]")
    converter = converter_class(ccode_object)
    cpp_obj = converter.to_cpp_code_object(conversion_data)

    debug.line("convert_ccode_object", f"Converted cpp_obj instance=[{cpp_obj}]")

    return cpp_obj
