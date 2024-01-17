
import code_object.code_interface as code_interface
import utils.debug as debug

# Representation of a C++ function
# The body is stored as lines of code (converted from cfunction's AST body)

class CppFunction(code_interface.CodeInterface):
    def __init__(self, funcsig, body) -> None:
        self._funcsig = funcsig
        self._body = body

    @property
    def funcsig(self):
        return self._funcsig

    # Return a string representing the funcsig as a declaration
    # Default is just funcsig - override to specialise
    @property
    def funcsig_as_declaration(self):
        return self._funcsig.as_string()

    # Return a string representing the funcsig as a definition
    # Default is just funcsig - override to specialise
    @property
    def funcsig_as_definition(self):
        return self._funcsig.as_string()

    @property
    def body(self):
        return self._body

    @body.setter
    def body(self, lines):
        self._body = lines

    # The whole function will be returned: signature and body (in braces)
    def as_lines(self):
        lines = [self.funcsig_as_definition]
        lines.extend(self._body)

        return lines
