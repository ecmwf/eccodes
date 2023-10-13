
from method_funcsig_conv import *
from funcsig import FuncSig

class PrivateMethodFuncSigConverter(MethodFuncSigConverter):
    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
