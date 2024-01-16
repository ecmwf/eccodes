import utils.debug as debug
from code_object.declaration_specifier import DeclSpec
import code_object.code_interface as code_interface

# Represent a struct argument
# Note - originally this was called struct.Struct but struct.py was causing a clash!
class StructArg(code_interface.CodeInterface):
    # Note: decl_spec can be a string or an instance of class DeclSpec
    def __init__(self, name, members=[]) -> None:
        self._name = name
        self._members = members

    @property
    def name(self):
        return self._name

    @property
    def members(self):
        return self._members

    # Each entry should be an Arg
    def add_member(self, member):
        self._members.append(member)

    def as_lines(self):
        lines = [f"struct {self._name} {{"]

        for m in self._members:
            lines.append(f"{m.as_string()};")

        lines.append("};")

        return lines
