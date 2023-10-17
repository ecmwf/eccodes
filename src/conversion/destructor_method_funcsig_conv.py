
from method_funcsig_conv import *
from funcsig import FuncSig
from arg import Arg

class DestructorMethodFuncSigConverter(MethodFuncSigConverter):
    destructor_method_conversions = [
        # static void destroy(grib_context*, grib_accessor*);
        FuncSigMapping(FuncSig("", "destroy", [Arg("grib_context*", ""), Arg("grib_accessor*", "")]),
                       FuncSig(None, None, [None, None]) ),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.destructor_method_conversions)
