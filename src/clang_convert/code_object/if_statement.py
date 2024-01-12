import debug
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines

# Represents an if statement in the form:
#
# if (expression) action else [...]
#
# [...] represents a child node, which could be another if, another action or None
#
# expression & action can be a string or a code_interface subclass (or None)
class IfStatement(code_interface.CodeInterface):
    def __init__(self, expression, action) -> None:
        if isinstance(expression, str):
            self._expression = code_lines.CodeLines(expression)
        else:
            self._expression = expression
        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class (or a string)"

        if isinstance(action, str):
            self._action = code_lines.CodeLines(action)
        else:
            self._action = action
        assert isinstance(self._action, code_interface.CodeInterface), f"Action must be a CodeInterface class (or a string)"

        self._else = None

    @property
    def expression(self):
        return self._expression

    @property
    def action(self):
        return self._action

    def add_else(self, else_statement):

        if isinstance(else_statement, str):
            self._else = code_lines.CodeLines(else_statement)
        else:
            self._else = else_statement
        assert isinstance(self._else, code_interface.CodeInterface), f"Else statement must be a CodeInterface class (or a string)"

    def as_lines(self):
        if_lines = self._expression.as_lines()
        if_lines[0] = "if (" + if_lines[0]
        if_lines[-1] += ")"

        action_lines = self._action.as_lines()

        if self._else:
            else_lines = self._else.as_lines()
            else_lines[0] = "else " + else_lines[0]
        else:
            else_lines = []

        return if_lines + action_lines + else_lines