
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE
from code_object.data_member import DataMember
from code_object.struct_member_access import struct_member_access_from_string

class BufrDataElementConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._header_includes.extend([
            "GribCpp/BufrDefs.h",
        ])

        self._data_member_mappings.update({
            DataMember("bufr_descriptors_array*", "descriptors"): DataMember("BufrDescriptorsArray", "descriptors"),
            })
        
        self._all_function_struct_member_access_mappings.update({
            struct_member_access_from_string("creator.op")          : struct_member_access_from_string("creator.op_"),
            struct_member_access_from_string("creator.name_space")  : struct_member_access_from_string("creator.nameSpace_"),
            struct_member_access_from_string("creator.set")         : struct_member_access_from_string("creator.set_"),
            struct_member_access_from_string("creator.name")        : struct_member_access_from_string("creator.name_"),
        })
