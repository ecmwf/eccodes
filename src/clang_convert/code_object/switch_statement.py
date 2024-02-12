import utils.debug as debug
import code_object.code_interface as code_interface
from utils.string_funcs import strip_semicolon

# Represents a switch statement in the form: switch (condition) statement
#
# statement is typically a compound statement containing case statements
class SwitchStatement(code_interface.CodeInterface):
    def __init__(self, condition, statement) -> None:
        self._condition = condition
        assert isinstance(self._condition, code_interface.CodeInterface), f"Condition must be a CodeInterface class"

        self._statement = statement
        assert isinstance(self._statement, code_interface.CodeInterface), f"Statement must be a CodeInterface class"

        self._else = None

    @property
    def condition(self):
        return self._condition

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        condition_lines = [f"switch({strip_semicolon(self._condition.as_string())})"]

        statement_lines = []
        statement_lines.extend(self._statement.as_lines())

        return condition_lines + statement_lines