import utils.debug as debug
import code_object.arg as arg
import code_object.code_interface as code_interface

# Represent a struct argument
# Note - originally this was called struct.Struct but struct.py was causing a clash!
#
# Name is a string
# members is a list of Arg classes
class StructArg(code_interface.CodeInterface):
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
        assert isinstance(member, arg.Arg), f"Expected Arg, got type=[{type(member)}]"
        self._members.append(member)

    def as_lines(self):
        lines = [f"struct {self._name} {{"]

        for m in self._members:
            lines.append(f"{m.as_string()};")

        lines.append("};")

        return lines
