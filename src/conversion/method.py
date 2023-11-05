
from func import *

# Specialisation of Function for AccessorData methods
class Method(Function):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

        self._const = False
        self.using = []

    @property
    def const(self):
        return "const" if self._const else ""

    @const.setter
    def const(self, value):
        self._const = value

    # Return all arg names in a string, for example to use to call the parent virtual function
    @property
    def parent_call_arg_string(self):
        return ", ".join([f"{a.name}" for a in self.args if a])
