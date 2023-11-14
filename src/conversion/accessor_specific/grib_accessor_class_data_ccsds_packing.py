from accessor_specific.default import AccessorSpecific

from converter_collection import Converter
import private_method_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
import arg
from funcsig_mapping import FuncSigMapping

class DataCcsdsPackingDataFuncSigConverter(private_method_funcsig_conv.PrivateMethodFuncSigConverter):
    func_conversions = [
        # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
        FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpackHelper", [None, Arg("std::vector<T>&", "vecTValues"),  None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class DataCcsdsPackingDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()

        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","buf") : arg.Arg("AccessorDataPointer","buf"),
            }


    def update_converters(self, converters):
        converters[Converter.PRIVATE_METHOD_FUNCSIG] = DataCcsdsPackingDataFuncSigConverter
        return converters
