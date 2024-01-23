
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
import code_object_converter.supporting.conversion_data as conversion_data

common_grib_type_transforms = {
    "grib_accessor*"    : "AccessorPtr",
    "grib_handle*"      : None,
    "grib_context*"     : None,
}

grib_array_type_transforms = {
    "grib_darray*"  : "std::vector<double>",
    "grib_iarray*"  : "std::vector<long>",
    "grib_sarray*"  : "std::string",
    "grib_vdarray*" : "std::vector<std::vector<double>>",
    "grib_viarray*" : "std::vector<std::vector<long>>",
    "grib_vsarray*" : "std::vector<std::string>",
}

grib_array_function_transforms = {
    "grib_darray"  : "std::vector<double>",
    "grib_iarray"  : "std::vector<long>",
    "grib_sarray"  : "std::string",
    "grib_vdarray" : "std::vector<std::vector<double>>",
    "grib_viarray" : "std::vector<std::vector<long>>",
    "grib_vsarray" : "std::vector<std::string>",
}

def add_grib_types_to_conversion_data(conversion_data):
    for type_transforms in [common_grib_type_transforms, grib_array_type_transforms]:
        for key, value in type_transforms.items():
            cdecl_spec = DeclSpec.from_decl_specifier_seq(key)
            cppdecl_spec = DeclSpec.from_decl_specifier_seq(value)
            conversion_data.add_type_mapping(cdecl_spec, cppdecl_spec)

    # TODO - Add Function Transforms...