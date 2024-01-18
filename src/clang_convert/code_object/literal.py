import utils.debug as debug
import code_object.code_interface as code_interface

# Represents any literal: integer, float, char, string (quoted)
class Literal(code_interface.CodeInterface):
    def __init__(self, value) -> None:
        self._value = value

    @property
    def value(self):
        return self._value

    def as_lines(self):
        return [f"{self._value}"]