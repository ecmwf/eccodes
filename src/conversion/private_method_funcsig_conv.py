
from method_funcsig_conv import *
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

class PrivateMethodFuncSigConverter(MethodFuncSigConverter):
    private_method_conversions = [
        # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
        FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpackHelper", [None, Arg("std::vector<T>&", "vecTValues"),  None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.private_method_conversions)
