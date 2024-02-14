import utils.debug as debug
import code_object.code_interface as code_interface

# Represent a catch statement in the form: catch(EXPRESSION) STATEMENT
#
class CatchStatement(code_interface.CodeInterface):
    def __init__(self, expression, statement) -> None:
        self._expression = expression
        self._statement = statement

        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class"
        assert isinstance(self._statement, code_interface.CodeInterface), f"Statement must be a CodeInterface class"

    @property
    def expression(self):
        return self._expression

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        lines = [f"catch({self._expression.as_string()})"]

        assert self._statement
        lines.extend(self._statement.as_lines())

        return lines