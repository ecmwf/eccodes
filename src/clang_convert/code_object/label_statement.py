import utils.debug as debug
import code_object.code_interface as code_interface

# Represents a label statement in the form: label: statement
#
# For example:
#   the_end:
#       GRIB_MUTEX_UNLOCK(&mutex1);
#
class LabelStatement(code_interface.CodeInterface):
    def __init__(self, label, statement) -> None:
        self._label = label
        assert isinstance(self._label, code_interface.CodeInterface), f"Label must be a CodeInterface class"

        self._statement = statement
        assert isinstance(self._statement, code_interface.CodeInterface), f"Statement must be a CodeInterface class"

    @property
    def label(self):
        return self._label

    @property
    def statement(self):
        return self._statement

    def as_lines(self):
        label_lines = [f"{self._label.as_string()}:"]

        statement_lines = []
        statement_lines.extend(self._statement.as_lines())

        return label_lines + statement_lines
    