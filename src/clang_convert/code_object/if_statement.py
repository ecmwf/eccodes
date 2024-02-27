import utils.debug as debug
import code_object.code_interface as code_interface
from utils.string_funcs import strip_semicolon

# Represents an if statement in the form:
#
# if (expression) action else [...]
#
# [...] represents a child node, which could be another if, another action or None
#
# expression & action can be a string or a code_interface subclass (or None)
class IfStatement(code_interface.CodeInterface):
    def __init__(self, expression, action, else_statement) -> None:
        self._expression = expression
        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class"

        self._action = action
        assert isinstance(self._action, code_interface.CodeInterface), f"Action must be a CodeInterface class"

        self._else_statement = else_statement
        assert self._else_statement is None or isinstance(self._else_statement, code_interface.CodeInterface), f"Else statement must be a CodeInterface class"

    @property
    def expression(self):
        return self._expression

    @property
    def action(self):
        return self._action

    @property
    def else_statement(self):
        return self._else_statement

    def as_lines(self):
        if_lines = []
        if_lines.extend(self._expression.as_lines())
        if_lines[0] = "if (" + if_lines[0]
        if_lines[-1] += ")"

        for i in range(len(if_lines)):
            if_lines[i] = if_lines[i].replace(";","")

        action_lines = []
        action_lines.extend(self._action.as_lines())

        else_lines = []
        if self._else_statement:
            else_lines.extend(self._else_statement.as_lines())
            else_lines[0] = "else " + else_lines[0]

        return if_lines + action_lines + else_lines