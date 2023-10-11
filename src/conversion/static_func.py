
from func import *

# Specialisation of Function for global functions
class StaticFunction(Function):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

