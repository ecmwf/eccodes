import utils.debug as debug
import code_object.code_interface as code_interface

# Represents an expression that refers to some value declaration, such as a function,
# variable, or enumerator.
#
# Models the LibClang DECL_REF_EXPR
class ValueDeclarationReference(code_interface.CodeInterface):
    def __init__(self, value) -> None:
        self._value = value

    @property
    def value(self):
        return self._value

    def as_lines(self):
        return [f"{self._value}"]