
from method import *

# Specialisation of AccessorData method
class PrivateMethod(Method):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)
