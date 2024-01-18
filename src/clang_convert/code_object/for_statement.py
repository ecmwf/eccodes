import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.literal as literal

# Represents a for loop in the form:
#
# for(init_statement; condition; iteration_expression) statement
#
# init_statement, condition and iteration_expression must be a code_interface subclass (or None)
class ForStatement(code_interface.CodeInterface):
    def __init__(self, init_statement, condition, iteration_expression, statement) -> None:
        if isinstance(init_statement, code_interface.CodeInterface):
            self._init_statement = init_statement
        elif init_statement is None:
            self._init_statement = literal.Literal("")
        else:
            assert False, f"Init statement must be a CodeInterface class (or None)"

        if isinstance(condition, code_interface.CodeInterface):
            self._condition = condition
        elif condition is None:
            self._condition = literal.Literal("")
        else:
            assert False, f"Condition must be a CodeInterface class (or None)"

        if isinstance(iteration_expression, code_interface.CodeInterface):
            self._iteration_expression = iteration_expression
        elif iteration_expression is None:
            self._iteration_expression = literal.Literal("")
        else:
            assert False, f"Iteration expression must be a CodeInterface class (or None)"

        if isinstance(statement, code_interface.CodeInterface):
            self._statement = statement
        elif statement is None:
            self._statement = literal.Literal(";")
        else:
            assert False, f"Statement must be a CodeInterface class (or None)"

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
        init_string = cond_string = iter_string = ""
        if self._init_statement:
            init_string = self._init_statement.as_string()
        if not init_string.endswith(";"):
            init_string += ";"
        if self._condition:
            cond_string = self._condition.as_string()
        if not cond_string.endswith(";"):
            cond_string += ";"
        if self._iteration_expression:
            iter_string = self._iteration_expression.as_string()

        lines = [f"for({init_string}{cond_string}{iter_string})"]

        if self._statement:
            lines.extend(self._statement.as_lines())
        else:
            lines.append(";")

        return lines