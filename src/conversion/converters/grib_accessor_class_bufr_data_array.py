
from converter_collection import Converter
import static_func_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

def update_converters(converters):
    converters[Converter.STATIC_FUNC_FUNCSIG] = BufrDataArrayFuncSigConverter
    return converters

class BufrDataArrayFuncSigConverter(static_func_funcsig_conv.StaticFunctionFuncSigConverter):
    func_conversions = [
        # static int check_overridden_reference_values(const grib_context* c, long* refValList, size_t refValListSize, int numBits)
        FuncSigMapping(FuncSig("int", "check_overridden_reference_values", [Arg("const grib_context*", "c"), Arg("long*", "refValList"), Arg("size_t", "refValListSize"), Arg("int", "numBits")]),
                       FuncSig("GribStatus", "checkOverriddenReferenceValues", [None, Arg("std::vector<long>", "refValList"), None, Arg("int", "numBits")]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)
