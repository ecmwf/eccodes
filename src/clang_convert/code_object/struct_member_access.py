
import code_object.code_interface as code_interface
import code_object.literal as literal
import code_object.array_access as array_access
import utils.debug as debug
from code_object.code_interface import NONE_VALUE
import re

# Represent the accessing of a struct member as defined in code, in order to manipulate/transform it
#
# It is recursive to allow structs within structs to be represented
#
# For example: *foo->bar->baz[4]
#
# At the top level: StructMemberAccess sa:
#   sa.access   = "*"
#   sa.variable = "foo"
#
# Then, one level down, is sa.member:
#   sa.member.access    = "->"
#   sa.member.variable  = "bar"
#
# There is one more level in this example: sa.member.member:
#   sa.member.member.access     = "->"
#   sa.member.member.variable   = "baz[4]"
#
class StructMemberAccess(code_interface.CodeInterface):
    def __init__(self, access, variable, member=None) -> None:
        self._access = access
        self._variable = variable
        self._member = member

        assert isinstance(self._variable, code_interface.CodeInterface), f"Variable must be a CodeInterface class"
        assert self._member is None or isinstance(self._member, code_interface.CodeInterface), f"Member must be a CodeInterface class, not [{type(self._member)}]"

    @property
    def access(self):
        return self._access

    @access.setter
    def access(self, value):
        self._access = value

    @property
    def variable(self):
        return self._variable

    @variable.setter
    def name(self, value):
        assert isinstance(value, code_interface.CodeInterface), f"Variable must be a CodeInterface class"
        self._variable = value

    @property
    def member(self):
        return self._member
    
    @member.setter
    def member(self, value):
        assert isinstance(value, StructMemberAccess), f"Member must be a StructMemberAccess class"
        self._member = value

    # ---------- Support for use as a dict key: Begin ----------
    def __hash__(self):
        return hash((self.access, self._variable, self._member))

    def __eq__(self, other):
        if self is NONE_VALUE or other is NONE_VALUE:
            return self is other

        # Ensure the other object is an instance of StructMemberAccess for comparison.
        if not isinstance(other, StructMemberAccess):
            return False

        return self.access == other.access and self.variable == other.variable and self.member == other.member
    # ---------- Support for use as a dict key: End   ----------


    def as_lines(self):
        access_str = self._access if self._access is not None else ""
        access_str += self._variable.as_string() if self._variable is not None else ""
        if self.member and self.member != NONE_VALUE:
            access_str += self.member.as_string()

        if access_str.endswith(")"):
            access_str += ";"

        return [access_str]

symbol_tokens = ["[", "]", "->", "."]
# Create from tokens e.g. ["a","->","b","->","c"]
def create_struct_member_access_from_tokens(tokens, depth=0):
    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}:IN] tokens={tokens}")

    assert len(tokens) != 0
    name = ""

    # Build the name from the non-symbol token(s)
    for tok in list(tokens):
        if tok not in symbol_tokens:
            name += tokens.pop(0)
        else:
            debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] name = [{debug.as_debug_string(name)}] tokens={tokens}")
            break
       
    assert name and name not in symbol_tokens, f"Name token can't be in {symbol_tokens}!"
    name_obj = literal.Literal(name)
    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] name_obj=[{debug.as_debug_string(name_obj)}] [{type(name_obj)}]")

    if len(tokens) == 0:
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}:OUT] No more tokens: name_obj=[{debug.as_debug_string(name_obj)}] [{type(name_obj)}]")
        return name_obj
    
    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}.1] tokens={tokens}")
    symbol = tokens.pop(0)

    if symbol == "[":
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] Found a [ - creating an array access object, tokens={tokens}")
        index_obj = create_struct_member_access_from_tokens(tokens, depth+1)
        array_access_obj = array_access.ArrayAccess(name_obj, index_obj)
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] array_access_obj=[{debug.as_debug_string(array_access_obj)}] [{type(array_access_obj)}] tokens={tokens}")
        name_obj = array_access_obj
        if len(tokens) == 0:
            return name_obj
        symbol = tokens.pop(0)

    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}.2] tokens={tokens}")

    if symbol == "]":
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}:OUT] Found ], name_obj=[{debug.as_debug_string(name_obj)}] tokens={tokens}")
        return name_obj

    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}.3] tokens={tokens}")

    if symbol in ["->", "."]:
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] Creating StructMemberAccess from tokens={tokens}")
        member_name_obj = create_struct_member_access_from_tokens(tokens, depth+1)
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] member_name_obj=[{debug.as_debug_string(member_name_obj)}]")
        member_obj = StructMemberAccess(symbol, member_name_obj)
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] member_obj=[{debug.as_debug_string(member_obj)}]")
        struct_member_access_obj = StructMemberAccess("", name_obj, member_obj)
        debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}:OUT] struct_member_access_obj=[{debug.as_debug_string(struct_member_access_obj)}] [{type(struct_member_access_obj)}]")
        return struct_member_access_obj

    debug.line("create_struct_member_access_from_tokens", f"{' ' * depth}[{depth}.4] tokens={tokens}")
    assert False, f"Didn't expect to get here! tokens={tokens}"

# Helper to create from a string e.g. "a->b->c->"
def struct_member_access_from_string(tokenstring, depth=0):

    # Create a regular expression pattern that captures any of the tokens
    # and also matches sequences of characters that are not the tokens
    # The pattern has two parts:
    # 1. A non-capturing group (?:...) with alternatives for each token, escaped as needed
    # 2. A segment to match any sequence of characters not matching the first part, using [^...]+
    pattern = '(' + '|'.join(re.escape(token) for token in symbol_tokens) + ')|([^' + ''.join(re.escape(token) for token in symbol_tokens) + ']+)'

    # Use re.findall() with the pattern to find all matches, including tokens
    matches = re.findall(pattern, tokenstring)

    # Flatten the resulting list of tuples and filter out empty strings
    tokens = [item for sublist in matches for item in sublist if item]

    return create_struct_member_access_from_tokens(tokens)
