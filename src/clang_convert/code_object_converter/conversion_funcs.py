
import code_object.array_access as arr_access
import code_object.arg as arg
import code_object.binary_operation as binary_operation
import code_object.code_objects as code_objects
import code_object.compound_statement as compound_statement
import code_object.conditional_operation as conditional_operation
import code_object.constructor_function as constructor_function
import code_object.declaration_specifier as declaration_specifier
import code_object.destructor_function as destructor_function
import code_object.for_statement as for_statement
import code_object.funcsig_pointer as funcsig_pointer
import code_object.funcsig as funcsig
import code_object.function as function
import code_object.function_call as function_call
import code_object.global_function as global_function
import code_object.if_statement as if_statement
import code_object.init_list as init_list
import code_object.literal as literal
import code_object.macro_definition as macro_definition
import code_object.macro_instantation as macro_instantation
import code_object.member_function as member_function
import code_object.operation as operation
import code_object.paren_expression as paren_expression
import code_object.return_statement as return_statement
import code_object.struct_arg as struct_arg
import code_object.struct_member_access as struct_member_access
import code_object.unary_expression as unary_expression
import code_object.unary_operation as unary_operation
import code_object.value_declaration_reference as value_declaration_reference
import code_object.variable_declaration as variable_declaration
import code_object.virtual_member_function as virtual_member_function

import code_object_converter.array_access_converter as array_access_converter
import code_object_converter.arg_converter as arg_converter
import code_object_converter.binary_operation_converter as binary_operation_converter
import code_object_converter.code_objects_converter as code_objects_converter
import code_object_converter.compound_statement_converter as compound_statement_converter
import code_object_converter.conditional_operation_converter as conditional_operation_converter
import code_object_converter.constructor_function_converter as constructor_function_converter
import code_object_converter.declaration_specifier_converter as declaration_specifier_converter
import code_object_converter.destructor_function_converter as destructor_function_converter
import code_object_converter.for_statement_converter as for_statement_converter
import code_object_converter.funcsig_converter as funcsig_converter
import code_object_converter.funcsig_pointer_converter as funcsig_pointer_converter
import code_object_converter.function_call_converter as function_call_converter
import code_object_converter.function_converter as function_converter
import code_object_converter.global_function_converter as global_function_converter
import code_object_converter.if_statement_converter as if_statement_converter
import code_object_converter.init_list_converter as init_list_converter
import code_object_converter.literal_converter as literal_converter
import code_object_converter.macro_definition_converter as macro_definition_converter
import code_object_converter.macro_instantiation_converter as macro_instantiation_converter
import code_object_converter.member_function_converter as member_function_converter
import code_object_converter.operation_converter as operation_converter
import code_object_converter.paren_expression_converter as paren_expression_converter
import code_object_converter.return_statement_converter as return_statement_converter
import code_object_converter.struct_arg_converter as struct_arg_converter
import code_object_converter.struct_member_access_converter as struct_member_access_converter
import code_object_converter.unary_expression_converter as unary_expression_converter
import code_object_converter.unary_operation_converter as unary_operation_converter
import code_object_converter.value_declaration_reference_converter as value_declaration_reference_converter
import code_object_converter.variable_declaration_converter as variable_declaration_converter
import code_object_converter.virtual_member_function_converter as virtual_member_function_converter

import code_object_converter.code_interface_converter as code_interface_converter
import utils.debug as debug
from utils.standard_transforms import transform_variable_name

# Mapping from C code objects to their converters
CodeInterfaceConverterClasses = {
    arr_access.ArrayAccess                                  : array_access_converter.ArrayAccessConverter,
    arg.Arg                                                 : arg_converter.ArgConverter,
    binary_operation.BinaryOperation                        : binary_operation_converter.BinaryOperationConverter,
    code_objects.CodeObjects                                : code_objects_converter.CodeObjectsConverter,
    compound_statement.CompoundStatement                    : compound_statement_converter.CompoundStatementConverter,
    conditional_operation.ConditionalOperation              : conditional_operation_converter.ConditionalOperationConverter,
    constructor_function.ConstructorFunction                : constructor_function_converter.ConstructorFunctionConverter,
    declaration_specifier.DeclSpec                          : declaration_specifier_converter.DeclSpecConverter,
    destructor_function.DestructorFunction                  : destructor_function_converter.DestructorFunctionConverter,
    for_statement.ForStatement                              : for_statement_converter.ForStatementConverter,
    funcsig.FuncSig                                         : funcsig_converter.FuncSigConverter,
    funcsig_pointer.FuncSigPointer                          : funcsig_pointer_converter.FuncSigPointerConverter,
    function_call.FunctionCall                              : function_call_converter.FunctionCallConverter,
    function.Function                                       : function_converter.FunctionConverter,
    global_function.GlobalFunction                          : global_function_converter.GlobalFunctionConverter,
    if_statement.IfStatement                                : if_statement_converter.IfStatementConverter,
    init_list.InitList                                      : init_list_converter.InitListConverter,
    literal.Literal                                         : literal_converter.LiteralConverter,
    member_function.MemberFunction                          : member_function_converter.MemberFunctionConverter,
    macro_definition.MacroDefinition                        : macro_definition_converter.MacroDefinitionConverter,
    macro_instantation.MacroInstantation                    : macro_instantiation_converter.MacroInstantiationConverter,
    operation.Operation                                     : operation_converter.OperationConverter,
    paren_expression.ParenExpression                        : paren_expression_converter.ParenExpressionConverter,
    return_statement.ReturnStatement                        : return_statement_converter.ReturnStatementConverter,
    struct_arg.StructArg                                    : struct_arg_converter.StructArgConverter,
    struct_member_access.StructMemberAccess                 : struct_member_access_converter.StructMemberAccessConverter,
    unary_expression.UnaryExpression                        : unary_expression_converter.UnaryExpressionConverter,
    unary_operation.UnaryOperation                          : unary_operation_converter.UnaryOperationConverter,
    value_declaration_reference.ValueDeclarationReference   : value_declaration_reference_converter.ValueDeclarationReferenceConverter,
    variable_declaration.VariableDeclaration                : variable_declaration_converter.VariableDeclarationConverter,
    virtual_member_function.VirtualMemberFunction           : virtual_member_function_converter.VirtualMemberFunctionConverter,
}

def get_debug_string(code_obj):
    if code_obj:
        return f"{code_obj if type(code_obj) is str else code_obj.as_string()}"
    else:
        return "None"

# Convert a code_object into a C++ code_object
def convert_ccode_object(ccode_object, conversion_data):
    debug.line("convert_ccode_object", f"[IN] [{type(ccode_object).__name__}] {get_debug_string(ccode_object)}")

    if ccode_object is None:
        cpp_obj = None
    elif isinstance(ccode_object, str):
        cpp_obj = transform_variable_name(ccode_object)
    else:
        converter_class = CodeInterfaceConverterClasses.get(type(ccode_object), code_interface_converter.CodeInterfaceConverter)
        converter = converter_class(ccode_object)
        cpp_obj = converter.to_cpp_code_object(conversion_data)

    debug.line("convert_ccode_object", f"[OUT][{type(cpp_obj).__name__}] {get_debug_string(cpp_obj)}")

    return cpp_obj
