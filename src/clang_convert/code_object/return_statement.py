import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.arg as arg

# Represent a return statement of the form return EXPR;
#
# EXPR is the expression being returned, which can be None to mean: return;
class ReturnStatement(code_interface.CodeInterface):
    def __init__(self, expression) -> None:
        self._expression = expression

        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class (or a string)"

    @property
    def expression(self):
        return self._expression

    def as_lines(self):
        return_string = "return"

        if self._expression:
            return_string += " " + self._expression.as_string()
        
        lines = [f"{return_string}"]

        if not lines[-1].endswith(";"):
            lines[-1] += ";"

        return lines