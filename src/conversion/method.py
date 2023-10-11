
from func import *

# Specialisation of Function for AccessorData methods
class Method(Function):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

        self._const = False
        #self._owner_arg_type = self._owner_class.name
        #self._owner_arg_name = arg.transform_variable_name(self._owner_arg_type)

    @property
    def const(self):
        return "const" if self._const else ""

    @const.setter
    def const(self, value):
        self._const = value
