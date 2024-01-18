import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines
import code_object.arg as arg

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

        assert isinstance(self._variable, arg.Arg), f"Variable must be an Arg class"
        assert isinstance(self._value, code_interface.CodeInterface), f"Value must be a CodeInterface class (or a string)"

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
        lines[-1] += ";"

        return lines