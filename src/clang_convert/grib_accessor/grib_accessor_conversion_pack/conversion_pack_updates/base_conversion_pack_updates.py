
import utils.debug as debug
from code_object.data_member import DataMember
from code_object.arg import Arg
from code_object.code_interface import NONE_VALUE

class BaseConversionPackUpdates:
    def __init__(self) -> None:
        self._update_funcs = {}
        self._funcsig_mappings = []
        self._all_function_arg_mappings = {}
        self._header_includes = []
        self._source_includes = []
        self._data_member_mappings = {
            DataMember("const char*", "cclass->name"): DataMember("std::string", "name_"),
        }
        self._all_function_struct_member_access_mappings = {}

    def add_accessor_wide_conversion_data(self, conversion_data):
        for mapping in self._funcsig_mappings:
            debug.line("add_funcsig_mappings_to_conversion_data", f"Adding funcsig mapping: [{mapping.cfuncsig.name}] -> [{mapping.cppfuncsig.name}]")
            conversion_data.add_global_funcsig_mapping(mapping)

        for cmember, cppmember in self._data_member_mappings.items():
            conversion_data.add_data_member_mapping(cmember, cppmember)

        for entry in self._header_includes:
            conversion_data.info.add_header_include(entry)

        for entry in self._source_includes:
            conversion_data.info.add_source_include(entry)


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
        # Add super class as data member
        conversion_pack.conversion_data.add_data_member_mapping(DataMember("grib_accessor_class**", "cclass->super"),
                                                                DataMember("AccessorData", 
                                                                           conversion_pack.conversion_data.info.super_class_name))

        # All function arg mappings
        for carg, cpparg in self._all_function_arg_mappings.items():
            conversion_pack.conversion_data.add_funcbody_arg_mapping(carg, cpparg)

        for cstruct_member_access, cppstruct_member_access in self._all_function_struct_member_access_mappings.items():
            conversion_pack.conversion_data.add_struct_member_access_mapping(cstruct_member_access, cppstruct_member_access)
