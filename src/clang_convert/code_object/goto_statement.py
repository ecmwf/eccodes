import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.arg as arg

# Represent a goto statement of the form goto LABEL;
#
class GotoStatement(code_interface.CodeInterface):
    def __init__(self, label) -> None:
        self._label = label

        assert isinstance(self._label, code_interface.CodeInterface), f"Label must be a CodeInterface class (or a string)"

    @property
    def label(self):
        return self._label

    def as_lines(self):
        lines = [f"goto {self._label.as_string()};"]

        return lines
    