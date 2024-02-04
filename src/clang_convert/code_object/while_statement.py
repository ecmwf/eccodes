import utils.debug as debug
import code_object.code_interface as code_interface
from utils.string_funcs import strip_semicolon

# Represents a while statement in the form: while (expression) action
#
class WhileStatement(code_interface.CodeInterface):
    def __init__(self, expression, action) -> None:
        self._expression = expression
        assert isinstance(self._expression, code_interface.CodeInterface), f"Expression must be a CodeInterface class"

        self._action = action
        assert isinstance(self._action, code_interface.CodeInterface), f"Action must be a CodeInterface class"

        self._else = None

    @property
    def expression(self):
        return self._expression

    @property
    def action(self):
        return self._action

    def as_lines(self):
        while_lines = []
        while_lines.extend(self._expression.as_lines())
        while_lines[0] = "while (" + while_lines[0]
        while_lines[-1] += ")"

        for i in range(len(while_lines)):
            while_lines[i] = while_lines[i].replace(";","")

        action_lines = []
        action_lines.extend(self._action.as_lines())

        return while_lines + action_lines