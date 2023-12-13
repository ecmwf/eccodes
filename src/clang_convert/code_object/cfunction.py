
import code_object.code_interface as code_interface
import debug

# Representation of a function
# The body is stored as an AST

class CFunction(code_interface.CodeInterface):
    def __init__(self, funcsig, body) -> None:
        self._funcsig = funcsig
        self._body = body

    @property
    def funcsig(self):
        return self._funcsig
    
    @property
    def body(self):
        return self._body
