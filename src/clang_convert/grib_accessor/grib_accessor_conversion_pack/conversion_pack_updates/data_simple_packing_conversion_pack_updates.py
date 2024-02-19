
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class DataSimplePackingConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._update_funcs.update({
            "number_of_bits": self.apply_updates_for_number_of_bits
        })

        self._funcsig_mappings.extend([
            # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
            FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                           FuncSig("GribStatus", "unpackHelper", [NONE_VALUE, Arg("std::vector<T>&", "vecTValues"),  NONE_VALUE]),
                           ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
        ])

        self._all_function_arg_mappings.update({
            Arg("unsigned char*","buf") : Arg("AccessorDataPointer","buf"),
        })

    def apply_updates_for_number_of_bits(self, conversion_pack):
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("const unsigned long*","n"), Arg("UnsignedLongVectorConstIterator","n"))
