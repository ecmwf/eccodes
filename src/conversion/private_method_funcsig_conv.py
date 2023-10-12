
from method_funcsig_conv import *
from funcsig import FuncSig
from arg import Arg

class PrivateMethodFuncSigConverter(MethodFuncSigConverter):
    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
