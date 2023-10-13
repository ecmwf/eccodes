
from funcsig_conv import *
import funcsig

global_cfuncsig = funcsig.FuncSig("void", "Global", [])

class GlobalFunctionFuncSigConverter(FuncSigConverter):
    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
