
from method import *

# Specialisation of AccessorData method
class ConstructorMethod(Method):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

    # Return all arg names in a string, for example to use to call the base constructor
    @property
    def arg_name_string(self):
        return ", ".join([f"{a.name}" for a in self.args if a])

