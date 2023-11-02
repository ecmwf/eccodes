
from method_funcsig_conv import *
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg

class ConstructorMethodFuncSigConverter(MethodFuncSigConverter):
    constructor_method_conversions = [
        # static void init(grib_accessor*, const long, grib_arguments*);
        FuncSigMapping(FuncSig("", "init", [Arg("grib_context*", ""), Arg("const long", ""), Arg("grib_arguments*", "")]),
                       FuncSig("Constructor", None,   [None, None, Arg("AccessorInitData const&", "initData")]),
                       ArgIndexes(cbuffer=2, clength=1, cpp_container=2) ),
        #FuncSigMapping(FuncSig("", "init", [Arg("grib_context*", ""), Arg("const long", "len"), Arg("grib_arguments*", "")]),
                       #FuncSig("Constructor", None,   [None, Arg("long", "initData.length"), Arg("AccessorInitData const&", "initData")])),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.constructor_method_conversions)
