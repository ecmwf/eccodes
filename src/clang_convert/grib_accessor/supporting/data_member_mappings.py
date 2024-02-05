
from code_object.data_member import DataMember
from code_object.declaration_specifier import DeclSpec
import code_object_converter.conversion_pack.conversion_data as conversion_data

# Define base class member mapping
grib_accessor_base_data_members_map = {
    DataMember("grib_buffer*","buffer")         : DataMember("AccessorDataView","buffer_"),
    DataMember("const char*","name")            : DataMember("std::string","name_"),
    DataMember("long","length")                 : DataMember("long","length_"),
    DataMember("long","offset")                 : DataMember("long","offset_"),
    DataMember("unsigned long","flags")         : DataMember("unsigned long","flags_"),
    DataMember("int","dirty")                   : DataMember("int","dirty_", mutable=True),
    DataMember("grib_virtual_value*","vvalue")  : DataMember("GribVirtualValuePtr","vvalue_"),
    DataMember("const char*","set")             : DataMember("std::string","set_"),
    }

def add_data_member_mappings_to_conversion_data(conversion_data):
    for funcbody_arg_mappings in [grib_accessor_base_data_members_map]:
        for key, value in funcbody_arg_mappings.items():
            conversion_data.add_data_member_mapping(key, value)
