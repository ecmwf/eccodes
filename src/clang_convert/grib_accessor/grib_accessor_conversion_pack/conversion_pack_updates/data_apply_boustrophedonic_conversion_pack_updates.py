
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class DataApplyBoustrophedonicConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._update_funcs.update({
            "unpack_helper": self.apply_updates_for_unpack_helper,
            "pack_double": self.apply_updates_for_pack_double,
        })

        self._funcsig_mappings.extend([
            # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
            FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                        FuncSig("GribStatus", "unpackHelper", [NONE_VALUE, Arg("std::vector<T>&", "vecTValues"),  NONE_VALUE]),
                        ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
        ])

    def apply_updates_for_unpack_helper(self, conversion_pack):
        for key,value in {
            Arg("double*","pvalues")    : Arg("double*","pvalues"),
            Arg("T*","pval")            : Arg("T*","pval"),
        }.items():
            conversion_pack.conversion_data.add_funcbody_arg_mapping(key, value)

    def apply_updates_for_pack_double(self, conversion_pack):
        for key,value in {
            Arg("double*","pvalues")    : Arg("double*","pvalues"),
            Arg("double*","pval")       : Arg("const double*","pval"),
        }.items():
            conversion_pack.conversion_data.add_funcbody_arg_mapping(key, value)
