import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.arg as arg
from code_object.code_interface import NONE_VALUE

# Represent a variable declaration of the form VAR = VALUE
# Models the VAR_DECL CursorKind in libclang
#
# VAR   is the variable, stored as an Arg
# OP    is the operation (see class operation.Operation)
# VALUE is the value used by OP (stored as a code_interface subclass)
class VariableDeclaration(code_interface.CodeInterface):
    def __init__(self, variable, value=None) -> None:
        self._variable = variable
        self._value = value

        assert self._variable != NONE_VALUE, f"Variable can't be NoneValue!"
        assert isinstance(self._variable, arg.Arg), f"Variable must be an Arg class, not=[{variable}]"
        assert isinstance(self._value, code_interface.CodeInterface), f"Value must be a CodeInterface class, not=[{self._value}]"

    @property
    def variable(self):
        return self._variable

    @property
    def value(self):
        return self._value

    def as_lines(self):
        lines = [self._variable.as_string()]
        if self._value:
            value_lines = self._value.as_lines()
            lines[0] += f" = {value_lines[0]}"
            lines.extend(value_lines[1:])

        if not lines[-1].endswith(";"):
            lines[-1] += ";"

        return lines