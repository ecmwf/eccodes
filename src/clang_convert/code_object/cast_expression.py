import utils.debug as debug
import code_object.code_interface as code_interface
from utils.string_funcs import strip_semicolon

# Represent a cast expression form CAST EXPRESSION
#
# CAST can be:
# 1. C-style e.g. (char*)foo;
# 2. C++ style e.g. static_cast<char*>(foo)

cast_types = [
    "C",
    "static",
    "dynamic",
    "reinterpret"
]

class CastExpression(code_interface.CodeInterface):
    def __init__(self, cast_type, cast_value, expression) -> None:
        self._cast_type = cast_type
        self._cast_value = cast_value
        self._expression = expression
        assert self._cast_type in cast_types, f"Invalid cast type [{self._cast_type}]"
        assert isinstance(self._cast_value, code_interface.CodeInterface), f"Cast value must be a CodeInterface class"
        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class"

    @property
    def cast_type(self):
        return self._cast_type

    @property
    def cast_value(self):
        return self._cast_value

    @property
    def expression(self):
        return self._expression

    def as_lines(self):

        if self._cast_type == "C":
            cast_expression_string = f"({self._cast_value.as_string()}){strip_semicolon(self._expression.as_string())}"
        else:
            cast_expression_string = f"{self._cast_type}_cast<{self._cast_value.as_string()}>({strip_semicolon(self._expression.as_string())})"

        if not cast_expression_string.endswith(";"):
            cast_expression_string += ";"

        return [cast_expression_string]
