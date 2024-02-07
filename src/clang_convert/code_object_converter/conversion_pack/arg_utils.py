
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

# Try to extract a name value from the object, else return ""
def extract_name(cpp_obj):

    assert isinstance(cpp_obj, CodeInterface), f"Expected CodeInterfacce, got [{type(cpp_obj).__name__}]"

    if isinstance(cpp_obj, Literal):
        return cpp_obj.value
    elif isinstance(cpp_obj, Arg):
        return cpp_obj.name
    elif isinstance(cpp_obj, StructArg):
        return cpp_obj.name
    elif isinstance(cpp_obj, VariableDeclaration):
        return cpp_obj.variable
    elif isinstance(cpp_obj, ArrayAccess):
        return cpp_obj.name
    elif isinstance(cpp_obj, StructMemberAccess):
        return cpp_obj.name
    elif isinstance(cpp_obj, ValueDeclarationReference):
        return cpp_obj.value
    elif isinstance(cpp_obj, UnaryOperation):
        # Operand will a CodeInterface, so we need to recurse!
        return extract_name(cpp_obj.operand)

    return ""

# If the code_object has an Arg representation, then this will be returned,
# otherwise None
def to_cpparg(cpp_obj, conversion_data):
    cpparg = None

    assert isinstance(cpp_obj, CodeInterface), f"Expected CodeInterfacce, got [{type(cpp_obj).__name__}]"

    cppname = extract_name(cpp_obj)
    if cppname:
        cpparg = conversion_data.cpparg_for_cppname(cppname)

    assert cpparg is None or isinstance(cpparg, Arg), f"cpparg should be Arg, not [{type(cpparg).__name__}]"

    debug.line("to_cpparg", f"cpp_obj=[{debug.as_debug_string(cpp_obj)}] -> cpparg=[{debug.as_debug_string(cpparg)}]")
    return cpparg
