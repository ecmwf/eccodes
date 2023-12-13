
import code_object.code_interface as code_interface
import debug

# Representation of a C++ function
# The body is stored as lines of code (converted from cfunction's AST body)

class CppFunction(code_interface.CodeInterface):
    def __init__(self, funcsig, body=[]) -> None:
        self._funcsig = funcsig
        self._body = body

    @property
    def funcsig(self):
        return self._funcsig
    
    @property
    def body(self):
        return self._body

    @body.setter
    def body(self, lines):
        self._body = lines

    # The whole function will be returned: signature and body (in braces)
    def as_lines(self):
        lines = self._funcsig.as_lines()
        lines.append("{")
        lines.extend(self._body)
        lines.append("}")

        return lines
    