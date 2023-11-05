
from method import *

# Specialisation of AccessorData method
class InheritedMethod(Method):
    def __init__(self, func_sig, self_class=None) -> None:
        super().__init__(func_sig, self_class)

    @property
    def as_header_declaration(self):
        return super().as_header_declaration + " override"
