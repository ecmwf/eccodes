
from method import *

# Specialisation of AccessorData method
class PrivateMethod(Method):
    def __init__(self, func_sig, self_class=None) -> None:
        super().__init__(func_sig, self_class)

        # Assume const. Constness will be removed if any member variables are assigned to
        self._const = True
