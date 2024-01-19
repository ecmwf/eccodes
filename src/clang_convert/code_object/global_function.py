
import code_object.function as function
import utils.debug as debug

# Stores everything that is not modelled by any other function object
# Usually this is global scope declarations and definitions
#
# It is modelled as a function body (stored as AST nodes) for consistent parsing (i.e. funcsig is None)

class GlobalFunction(function.Function):
    def __init__(self, body) -> None:
        super().__init__(None, body)

    # Only the body will be returned (there's no signature!)
    def as_lines(self):
        lines = []
        lines.extend(self._body.as_lines())

        return lines
