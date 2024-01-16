import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines

# Represent a unary expression expression in the form KEYWORD(EXPRESSION)
# For example: sizeof(char)
#
# Models the CXX_UNARY_EXPR CursorKind in libclang
#
# keyword is a string
# expression can be a string or a code_interface subclass
class UnaryExpression(code_interface.CodeInterface):
    def __init__(self, keyword, expression) -> None:
        self._keyword = keyword

        if isinstance(expression, str):
            self._expression = code_lines.CodeLines(expression)
        else:
            self._expression = expression

        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class (or a string)"

    @property
    def keyword(self):
        return self._keyword

    @property
    def expression(self):
        return self._expression

    def as_lines(self):
        lines = []

        if self._expression:
            lines = self._expression.as_lines()
            lines[0] = self._keyword + lines[0]

        return lines