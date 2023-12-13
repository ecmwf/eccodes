
import code_object.code_interface as code_interface
import debug

# Represent the accessing of a struct member as defined in code, in order to manipulate/transform it
#
# It is recursive to allow structs within structs to be represented
#
# For example: *foo->bar->baz[4]
#
# At the top level: StructMemberAccess sa:
#   sa.access  = "*"
#   sa.name    = "foo"
#   sa.index   = ""
#
# Then, one level down, is sa.member:
#   sa.member.access  = "->"
#   sa.member.name    = "bar"
#   sa.member.index   = ""
#
# There is one more level in this example: sa.member.member:
#   sa.member.member.access  = "->"
#   sa.member.member.name    = "baz"
#   sa.member.member.index   = "[4]"
#
class StructMemberAccess(code_interface.CodeInterface):
    def __init__(self, access, name, index, member=None) -> None:
        self._access = access
        self._name = name
        self._index = index
        self._member = member

    @property
    def access(self):
        return self._access

    @access.setter
    def access(self, value):
        self._access = value

    @property
    def name(self):
        return self._name

    @property
    def index(self):
        return self._index
    
    @index.setter
    def index(self, value):
        assert value[0] == "[" and value[-1] == "]", f"Invalid index [{value}]"
        self._index = value

    @property
    def member(self):
        return self._member
    
    @member.setter
    def member(self, value):
        self._member = value

    def as_lines(self):
        access_str = self._access if self._access is not None else ""
        access_str += self._name if self._name is not None else ""
        access_str += self._index if self._index is not None else ""
        if self.member:
            access_str += self.member.as_string()

        return [access_str]
