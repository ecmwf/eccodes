
import debug

# Representation of a C++ function
# The body is stored as lines of code (converted from cfunction's AST body)

class CppFunction:
    def __init__(self, funcsig) -> None:
        self._funcsig = funcsig
        self._body = []

    @property
    def funcsig(self):
        return self._funcsig
    
    @property
    def body(self):
        return self._body
