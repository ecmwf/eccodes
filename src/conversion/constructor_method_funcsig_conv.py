
from method_funcsig_conv import *
from funcsig import FuncSig
from arg import Arg

class ConstructorMethodFuncSigConverter(MethodFuncSigConverter):
    constructor_method_conversions = [
        # static void init(grib_accessor*, const long, grib_arguments*);
        FuncSigMapping(FuncSig("", "init", [Arg("grib_context*", ""), Arg("const long", ""), Arg("grib_arguments*", "")]),
                       FuncSig("Constructor", None,   [None, None, Arg("AccessorInitData const&", "initData")]) ),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.constructor_method_conversions)
