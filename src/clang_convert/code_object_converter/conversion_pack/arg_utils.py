
import utils.debug as debug
from code_object.code_interface import CodeInterface
from code_object.literal import Literal
from code_object.arg import Arg
from code_object.struct_arg import StructArg
from code_object.variable_declaration import VariableDeclaration
from code_object.array_access import ArrayAccess
from code_object.struct_member_access import StructMemberAccess
from code_object.value_declaration_reference import ValueDeclarationReference


# If the code_object has an Arg representation, then this will be returned,
# otherwise None
def to_cpparg(cpp_obj, conversion_data):
    cpparg = None

    assert isinstance(cpp_obj, CodeInterface), f"Expected CodeInterfacce, got [{type(cpp_obj).__name__}]"

    if isinstance(cpp_obj, Literal):
        cpparg = conversion_data.cpparg_for_cppname(cpp_obj.value)
    elif isinstance(cpp_obj, Arg):
        cpparg = cpp_obj
    elif isinstance(cpp_obj, StructArg):
        cpparg = conversion_data.cpparg_for_cppname(cpp_obj.name)
    elif isinstance(cpp_obj, VariableDeclaration):
        cpparg = cpp_obj.variable
    elif isinstance(cpp_obj, ArrayAccess):
        cpparg = conversion_data.cpparg_for_cppname(cpp_obj.name)
    elif isinstance(cpp_obj, StructMemberAccess):
        cpparg = conversion_data.cpparg_for_cppname(cpp_obj.name)
    elif isinstance(cpp_obj, ValueDeclarationReference):
        cpparg = conversion_data.cpparg_for_cppname(cpp_obj.value)

    assert cpparg is None or isinstance(cpparg, Arg), f"cpparg should be Arg, not [{type(cpparg).__name__}]"

    debug.line("to_cpparg", f"cpp_obj=[{debug.as_debug_string(cpp_obj)}] -> cpparg=[{debug.as_debug_string(cpparg)}]")
    return cpparg
