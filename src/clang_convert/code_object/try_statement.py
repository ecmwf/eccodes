import utils.debug as debug
import code_object.code_interface as code_interface

# Represent a try statement in the form: try STATEMENT
# For example: try { int a = a_vec[22]; }
#
class TryStatement(code_interface.CodeInterface):
    def __init__(self, statement) -> None:
        self._statement = statement

        assert isinstance(self._statement, code_interface.CodeInterface), f"Statement must be a CodeInterface class"

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        lines = ["try"]

        assert self._statement
        lines.extend(self._statement.as_lines())

        return lines