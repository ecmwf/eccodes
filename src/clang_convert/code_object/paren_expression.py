import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines

# Represent an expression wrapped in a pair of parentheses, i.e. (EXPRESSION)
# Models the PAREN_EXPR CursorKind in libclang
#
# expression can be a string or a code_interface subclass
class ParenExpression(code_interface.CodeInterface):
    def __init__(self, expression) -> None:
        if isinstance(expression, str):
            self._expression = code_lines.CodeLines(expression)
        else:
            self._expression = expression

        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class (or a string)"

    @property
    def expression(self):
        return self._expression

    def as_lines(self):
        lines = []

        if self._expression:
            lines = self._expression.as_lines()
            lines[0] = "(" + lines[0]
            lines[-1] += ")"

        return lines