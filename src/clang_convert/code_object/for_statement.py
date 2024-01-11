import debug
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines

# Represents a for loop in the form:
#
# for(init_statement; condition; iteration_expression) statement
#
# init_statement, condition and iteration_expression can be a string or a code_interface subclass (or None)
class ForStatement(code_interface.CodeInterface):
    def __init__(self, init_statement, condition, iteration_expression, statement) -> None:
        if isinstance(init_statement, str):
            self._init_statement = code_lines.CodeLines(init_statement)
        elif isinstance(init_statement, code_interface.CodeInterface):
            self._init_statement = init_statement
        elif init_statement is None:
            self._init_statement = code_lines.CodeLines("")
        else:
            assert False, f"Init statement must be a CodeInterface class (or a string)"

        if isinstance(condition, str):
            self._condition = code_lines.CodeLines(condition)
        elif isinstance(condition, code_interface.CodeInterface):
            self._condition = condition
        elif condition is None:
            self._condition = code_lines.CodeLines("")
        else:
            assert False, f"Condition must be a CodeInterface class (or a string)"

        if isinstance(iteration_expression, str):
            self._iteration_expression = code_lines.CodeLines(iteration_expression)
        elif isinstance(iteration_expression, code_interface.CodeInterface):
            self._iteration_expression = iteration_expression
        elif iteration_expression is None:
            self._iteration_expression = code_lines.CodeLines("")
        else:
            assert False, f"Iteration expression must be a CodeInterface class (or a string)"

        if isinstance(statement, str):
            self._statement = code_lines.CodeLines(statement)
        elif isinstance(statement, code_interface.CodeInterface):
            self._statement = statement
        elif statement is None:
            self._statement = code_lines.CodeLines(";")
        else:
            assert False, f"Statement must be a CodeInterface class (or a string)"

    @property
    def init_statement(self):
        return self._init_statement

    @property
    def condition(self):
        return self._condition

    @property
    def iteration_expression(self):
        return self._iteration_expression

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        lines = ["for("]
        if self._init_statement:
            lines.extend(self._init_statement.as_lines())
        if not lines[-1].endswith(";"):
            lines[-1] += ";"
        if self._condition:
            lines.extend(self._condition.as_lines())
        if not lines[-1].endswith(";"):
            lines[-1] += ";"
        if self._iteration_expression:
            lines.extend(self._iteration_expression.as_lines())
        lines[-1] += ")"

        if self._statement:
            lines.extend(self._statement.as_lines())
        else:
            lines.append(";")

        return lines