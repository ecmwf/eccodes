
from funcsig_conv import *

class MethodFuncSigConverter(FuncSigConverter):
    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)

    # Overridden as function methods should not usually be static
    def is_cpp_static(self):
        return False
