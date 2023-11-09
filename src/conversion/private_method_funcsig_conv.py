
from method_funcsig_conv import *
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

class PrivateMethodFuncSigConverter(MethodFuncSigConverter):
    private_method_conversions = []

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.private_method_conversions)
