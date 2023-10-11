
from method import *
import member_function_transforms

# Specialisation of AccessorData method
class InheritedMethod(Method):
    def __init__(self, func_sig) -> None:
        super().__init__(func_sig)

