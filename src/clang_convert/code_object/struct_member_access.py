
import code_object.code_interface as code_interface
import utils.debug as debug
from code_object.code_interface import NONE_VALUE

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
        assert self._member is None or isinstance(self._member, code_interface.CodeInterface), f"Member must be a CodeInterface class"

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

    def as_lines(self):
        access_str = self._access if self._access is not None else ""
        access_str += self._variable.as_string() if self._variable is not None else ""
        if self.member and self.member != NONE_VALUE:
            access_str += self.member.as_string()

        if access_str.endswith(")"):
            access_str += ";"

        return [access_str]
