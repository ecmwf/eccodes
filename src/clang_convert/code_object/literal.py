import utils.debug as debug
import code_object.code_interface as code_interface
from code_object.code_interface import NONE_VALUE

# Represents any literal: integer, float, char, string (quoted)
class Literal(code_interface.CodeInterface):
    def __init__(self, value) -> None:
        self._value = value

    @property
    def value(self):
        return self._value

    # ---------- Support for use as a dict key: Begin ----------
    def __hash__(self):
        return hash((self.value))

    def __eq__(self, other):
        if self is NONE_VALUE or other is NONE_VALUE:
            return self is other

        # Ensure the other object is an instance of StructMemberAccess for comparison.
        if not isinstance(other, Literal):
            return False

        return self.value == other.value
    # ---------- Support for use as a dict key: End   ----------

    def as_lines(self):
        return [f"{self._value}"]