
from func import *

# Specialisation of Function for global functions
class GlobalFunction(Function):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

    # Overridden as there's no opening and closing { } so don't want to assert!
    @property
    def code(self):
        return self._lines
