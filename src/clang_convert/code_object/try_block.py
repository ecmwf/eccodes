import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.code_objects as code_objects
from code_object.try_statement import TryStatement

# Represent a try block encompassing a try statement and 1+ catch statements
#
class TryBlock(code_interface.CodeInterface):
    def __init__(self, try_statement, catch_statements) -> None:
        self._try_statement = try_statement
        self._catch_statements = catch_statements

        assert isinstance(self._try_statement, TryStatement), f"Try statement must be a TryStatement class"
        assert isinstance(self._catch_statements, code_objects.CodeObjects), f"Catch statements must be a CodeObjects class"

    @property
    def try_statement(self):
        return self._try_statement

    @property
    def catch_statements(self):
        return self._catch_statements

    def as_lines(self):
        lines = []
        lines.extend(self._try_statement.as_lines())
        lines.extend(self._catch_statements.as_lines())

        return lines