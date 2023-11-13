from accessor_specific.default import AccessorSpecific

from converter_collection import Converter
import static_func_funcsig_conv
import private_method_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
import arg
from funcsig_mapping import FuncSigMapping

class UnsignedDataFuncSigConverter(static_func_funcsig_conv.StaticFunctionFuncSigConverter):
    func_conversions = [
        FuncSigMapping(FuncSig("int", "value_is_missing", [Arg("long", "val")]),
                       FuncSig("int", "valueIsMissing", [Arg("long", "val")])),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class UnsignedDataPrivateMethodFuncSigConverter(private_method_funcsig_conv.PrivateMethodFuncSigConverter):
    func_conversions = [
        FuncSigMapping(FuncSig("int", "pack_long_unsigned_helper", [Arg("grib_accessor*", "a"), Arg("const long*", "val"), Arg("size_t*", "len"), Arg("int", "check")]),
                       FuncSig("GribStatus", "packLongUnsignedHelper", [None, Arg("std::vector<long> const&", "longValues"), None, Arg("int", "check")]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class UnsignedDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
    
        self._custom_arg_transforms = {
            arg.Arg("unsigned char*","buf") : arg.Arg("AccessorDataBuffer","buf"),
            }

        self._custom_member_arg_transforms = {
            arg.Arg("grib_arguments*","arg") : arg.Arg("AccessorInitData","arg"),
            }
        
    def update_converters(self, converters):
        converters[Converter.STATIC_FUNC_FUNCSIG] = UnsignedDataFuncSigConverter
        converters[Converter.PRIVATE_METHOD_FUNCSIG] = UnsignedDataPrivateMethodFuncSigConverter
        return converters
