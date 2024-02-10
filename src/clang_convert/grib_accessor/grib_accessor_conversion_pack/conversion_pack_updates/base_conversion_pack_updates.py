
import utils.debug as debug
from code_object.data_member import DataMember
from code_object.arg import Arg
from code_object.code_interface import NONE_VALUE

class BaseConversionPackUpdates:
    def __init__(self) -> None:
        self._update_funcs = {}

    # Use this entry point to call the appropriate derived function
    def apply_updates_for_cfunction(self, cfuncname, conversion_pack):

        self.apply_updates_for_all_functions(conversion_pack)

        for name, update_func in self._update_funcs.items():
            if name == cfuncname:
                update_func(conversion_pack)
                break

    # Override to set the same update for all functions
    # NOTE: Doesn't apply to global definition
    def apply_updates_for_all_functions(self, conversion_pack):
        # Default data member mappings
        for cmember, cppmember in {
            DataMember("grib_accessor_class**", "cclass->super"): DataMember("AccessorData", conversion_pack.conversion_data.info.super_class_name),
        }.items():
            conversion_pack.conversion_data.add_data_member_mapping(cmember, cppmember)
