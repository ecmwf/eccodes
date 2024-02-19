
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class UnsignedConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._funcsig_mappings.extend([
            FuncSigMapping(FuncSig("int", "pack_long_unsigned_helper", [Arg("grib_accessor*", "a"), Arg("const long*", "val"), Arg("size_t*", "len"), Arg("int", "check")]),
                           FuncSig("GribStatus", "packLongUnsignedHelper", [NONE_VALUE, Arg("std::vector<long> const&", "longValues"), NONE_VALUE, Arg("int", "check")]),
                           ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
        ])

        self._all_function_arg_mappings.update({
            Arg("unsigned char*","buf") : Arg("AccessorDataBuffer","buf"),
            # This may need to be a data member...
            Arg("grib_arguments*","arg") : Arg("AccessorInitData","arg"),
        })
