
import utils.debug as debug
from code_object.code_interface import CodeInterface
from code_object.literal import Literal
from code_object.arg import Arg
from code_object.struct_arg import StructArg
from code_object.variable_declaration import VariableDeclaration
from code_object.array_access import ArrayAccess
from code_object.struct_member_access import StructMemberAccess
from code_object.value_declaration_reference import ValueDeclarationReference
from code_object.unary_operation import UnaryOperation
from code_object.unary_expression import UnaryExpression
from code_object.paren_expression import ParenExpression
from utils.string_funcs import strip_semicolon
from code_object.cast_expression import CastExpression

# Try to extract a name value from the object, else return ""
def extract_name(cpp_obj):

    debug.line("extract_name", f"[IN]  cpp_obj=[{debug.as_debug_string(cpp_obj)}] type=[{type(cpp_obj)}]")

    assert isinstance(cpp_obj, CodeInterface), f"Expected CodeInterfacce, got [{type(cpp_obj).__name__}]"

    cppname = ""

    if isinstance(cpp_obj, Literal):
        cppname = cpp_obj.value
    elif isinstance(cpp_obj, Arg):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, StructArg):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, VariableDeclaration):
        cppname = cpp_obj.variable
    elif isinstance(cpp_obj, ArrayAccess):
        cppname = extract_name(cpp_obj.name)
    elif isinstance(cpp_obj, StructMemberAccess):
        if cpp_obj.name:
            cppname = extract_name(cpp_obj.name)
            
        if not cppname and cpp_obj.member.name:
            cppname = extract_name(cpp_obj.member.name)
    elif isinstance(cpp_obj, ValueDeclarationReference):
        cppname = cpp_obj.value
    elif isinstance(cpp_obj, UnaryOperation):
        # Operand will a CodeInterface, so we need to recurse!
        cppname = extract_name(cpp_obj.operand)
    elif isinstance(cpp_obj, UnaryExpression):
        # expression will a CodeInterface, so we need to recurse!
        cppname = extract_name(cpp_obj.expression)
    elif isinstance(cpp_obj, ParenExpression):
        # expression will a paren_expression, so we need to recurse!
        cppname = extract_name(cpp_obj.expression)
    elif isinstance(cpp_obj, CastExpression):
        # expression will a CodeInterface, so we need to recurse!
        cppname = extract_name(cpp_obj.expression)

    if not isinstance(cppname, str):
        debug.line("extract_name", f"** Could not extract string name. cppname type=[{type(cppname)}] value=[{debug.as_debug_string(cppname)}]")
        cppname = ""

    debug.line("extract_name", f"[OUT] cpp_obj=[{debug.as_debug_string(cpp_obj)}] -> cppname=[{cppname}]")

    return cppname

# If the code_object has an Arg representation, then this will be returned,
# otherwise None
def to_cpparg(cpp_obj, conversion_data):
    cpparg = None

    assert isinstance(cpp_obj, CodeInterface), f"Expected CodeInterfacce, got [{type(cpp_obj).__name__}]"

    name = extract_name(cpp_obj)
    if name:
        cpparg = conversion_data.cpparg_for_cppname(name)
        if not cpparg:
            cpparg = conversion_data.cpparg_for_cname(name)

    assert cpparg is None or isinstance(cpparg, Arg), f"cpp_obj=[{debug.as_debug_string(cpp_obj)}] : cpparg should be Arg, not [{type(cpparg).__name__}]"

    debug.line("to_cpparg", f"cpp_obj=[{debug.as_debug_string(cpp_obj)}] -> cpparg=[{debug.as_debug_string(cpparg)}]")
    return cpparg

# Return the argument in a form that can be used in a function call
def extract_function_call_name(cpp_obj):
    cppname = None

    if isinstance(cpp_obj, Literal):
        cppname = cpp_obj.value
    elif isinstance(cpp_obj, Arg):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, StructArg):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, VariableDeclaration):
        cppname = cpp_obj.variable
    '''elif isinstance(cpp_obj, ArrayAccess):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, StructMemberAccess):
        cppname = cpp_obj.name
    elif isinstance(cpp_obj, ValueDeclarationReference):
        cppname = cpp_obj.value
    elif isinstance(cpp_obj, UnaryOperation):
        # Operand will a CodeInterface, so we need to recurse!
        cppname = self.function_call_name(cpp_obj.operand)
    elif isinstance(cpp_obj, UnaryExpression):
        # expression will a CodeInterface, so we need to recurse!
        cppname = self.function_call_name(cpp_obj.expression)
    elif isinstance(cpp_obj, ParenExpression):
        # expression will a paren_expression, so we need to recurse!
        cppname = self.function_call_name(cpp_obj.expression)'''
    
    if not cppname:
        cppname = strip_semicolon(cpp_obj.as_string())

    debug.line("function_call_name", f"Converted cpp_obj: [{debug.as_debug_string(cpp_obj)}] -> [{cppname}] cpp_ob type=[{type(cpp_obj)}]")

    return cppname
