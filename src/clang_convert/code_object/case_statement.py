import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.code_objects as code_objects
from utils.string_funcs import strip_semicolon

# Represents a case statement in the form: case constant_expression: statement
#
# NOTE: lib-clang only gives us the first statement after the case label!
class CaseStatement(code_interface.CodeInterface):
    def __init__(self, constant_expression, statement) -> None:
        self._constant_expression = constant_expression
        assert isinstance(self._constant_expression, code_interface.CodeInterface), f"Constant expression must be a CodeInterface class"

        self._statement = statement
        assert isinstance(self._statement, code_interface.CodeInterface), f"Statement must be a CodeInterface class"

        self._else = None

    @property
    def constant_expression(self):
        return self._constant_expression

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        case_label = self._constant_expression.as_string()
        if case_label == "default":
            constant_expression_lines = [f"{case_label}:"]
        else:
            constant_expression_lines = [f"case {case_label}:"]

        statement_lines = []
        statement_lines.extend(self._statement.as_lines())

        return constant_expression_lines + statement_lines