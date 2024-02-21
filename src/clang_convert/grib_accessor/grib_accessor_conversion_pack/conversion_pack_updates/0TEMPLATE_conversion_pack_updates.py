
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE
from code_object.data_member import DataMember
from code_object.struct_member_access import create_struct_member_access_from_tokenstring

class TemplateConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._update_funcs.update({
            "pack_long": self.apply_updates_for_pack_long
        })

        self._header_includes.extend([
            "BufrDefs.h",
        ])

        self._source_includes.extend([
            "<float.h>",
        ])

        self._funcsig_mappings.extend([
            # template<typename T> static int unpack_helper(grib_accessor* a, T* val, size_t* len)
            FuncSigMapping(FuncSig("int", "unpack_helper", [Arg("grib_accessor*", "a"), Arg("T*", "val"), Arg("size_t*", "len")]),
                        FuncSig("GribStatus", "unpackHelper", [NONE_VALUE, Arg("std::vector<T>&", "vecTValues"),  NONE_VALUE]),
                        ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
        ])

        self._all_function_arg_mappings.update({
            Arg("unsigned char*","p") : Arg("AccessorDataPointer","p"),
        })

        self._data_member_mappings.update({
            DataMember("foo*", "bar"): DataMember("Foo", "bar_"),
            })

        self._all_function_struct_member_access_mappings.update({
            create_struct_member_access_from_tokenstring("foo->bar"): create_struct_member_access_from_tokenstring("foo_.bar_"),
        })

    def apply_updates_for_pack_long(self, conversion_pack):
        conversion_pack.conversion_data.add_funcbody_arg_mapping(Arg("unsigned char*","mdata"), Arg("AccessorDataPointer","mdata"))
