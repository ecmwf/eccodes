
from funcsig_conv import *

class StaticFunctionFuncSigConverter(FuncSigConverter):
    static_function_conversions = []

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.static_function_conversions)
