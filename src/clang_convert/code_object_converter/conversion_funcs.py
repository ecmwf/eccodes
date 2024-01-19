
import code_object.array_access as arr_access
import code_object.arg as arg
import code_object.binary_operation as binary_operation
import code_object.code_objects as code_objects
import code_object.compound_statement as compound_statement
import code_object.declaration_specifier as declaration_specifier
import code_object.for_statement as for_statement
import code_object.funcsig_pointer as funcsig_pointer
import code_object.funcsig as funcsig
import code_object.function as function
import code_object.function_call as function_call
import code_object.global_function as global_function
import code_object.variable_declaration as variable_declaration

import code_object_converter.array_access_converter as array_access_converter
import code_object_converter.arg_converter as arg_converter
import code_object_converter.binary_operation_converter as binary_operation_converter
import code_object_converter.code_objects_converter as code_objects_converter
import code_object_converter.compound_statement_converter as compound_statement_converter
import code_object_converter.declaration_specifier_converter as declaration_specifier_converter
import code_object_converter.for_statement_converter as for_statement_converter
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.funcsig_pointer_converter as funcsig_pointer_converter
import code_object_converter.function_call_converter as function_call_converter
import code_object_converter.function_converter as function_converter
import code_object_converter.global_function_converter as global_function_converter
import code_object_converter.variable_declaration_converter as variable_declaration_converter

import code_object_converter.code_interface_converter as code_interface_converter
import utils.debug as debug

# Mapping from C code objects to their converters
CodeInterfaceConverterClasses = {
    arr_access.ArrayAccess                      : array_access_converter.ArrayAccessConverter,
    arg.Arg                                     : arg_converter.ArgConverter,
    binary_operation.BinaryOperation            : binary_operation_converter.BinaryOperationConverter,
    code_objects.CodeObjects                    : code_objects_converter.CodeObjectsConverter,
    compound_statement.CompoundStatement        : compound_statement_converter.CompoundStatementConverter,
    declaration_specifier.DeclSpec              : declaration_specifier_converter.DeclSpecConverter,
    for_statement.ForStatement                  : for_statement_converter.ForStatementConverter,
    funcsig.FuncSig                             : funcsig_converter.FuncSigConverter,
    funcsig_pointer.FuncSigPointer              : funcsig_pointer_converter.FuncSigPointerConverter,
    function_call.FunctionCall                  : function_call_converter.FunctionCallConverter,
    function.Function                           : function_converter.FunctionConverter,
    global_function.GlobalFunction              : global_function_converter.GlobalFunctionConverter,
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
