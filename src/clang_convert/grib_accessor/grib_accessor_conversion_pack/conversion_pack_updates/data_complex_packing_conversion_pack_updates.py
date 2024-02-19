
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class DataComplexPackingConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._update_funcs.update({
            "unpack_helper" : self.apply_updates_for_unpack_helper,
            "pack_double"   : self.apply_updates_for_pack_double,
        })

        self._funcsig_mappings.extend([
            # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
            FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                           FuncSig("GribStatus", "unpackHelper", [NONE_VALUE, Arg("std::vector<T>&", "vecTValues"),  NONE_VALUE]),
                           ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
            FuncSigMapping(FuncSig("double", "calculate_pfactor", [Arg("const grib_context*", "a"), Arg("const double*", "spectralField"), Arg("long", "fieldTruncation"), Arg("long", "subsetTruncation")]),
                           FuncSig("double", "calculatePfactor", [NONE_VALUE, Arg("std::vector<double>", "spectralField"), Arg("long", "fieldTruncation"), Arg("long", "subsetTruncation")])),
        ])

    def apply_updates_for_unpack_helper(self, conversion_pack):
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","buf"), Arg("AccessorDataPointer","buf"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","hres"), Arg("AccessorDataPointer","hres"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","lres"), Arg("AccessorDataPointer","lres"))
        #conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","scals"), Arg("AccessorDataBuffer","scals"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("T*","pscals"), Arg("T","pscals"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("T*","pval"), Arg("T","pval"))

    def apply_updates_for_pack_double(self, conversion_pack):
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","buf"), Arg("AccessorDataBuffer","buf"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","hres"), Arg("AccessorDataPointer","hres"))
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","lres"), Arg("AccessorDataPointer","lres"))
