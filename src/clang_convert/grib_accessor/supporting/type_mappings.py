
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
import code_object_converter.conversion_pack.conversion_data as conversion_data

common_grib_type_mappings = {
    "grib_accessor*"    : "AccessorPtr",
    "grib_handle*"      : DeclSpec.NONE,
    "grib_context*"     : DeclSpec.NONE,
    "grib_expression*"  : "GribExpressionPtr",
    "grib_codetable*"   : "GribCodeTablePtr",
}

grib_array_type_mappings = {
    "grib_darray*"  : "std::vector<double>",
    "grib_iarray*"  : "std::vector<long>",
    "grib_sarray*"  : "std::string",
    "grib_vdarray*" : "std::vector<std::vector<double>>",
    "grib_viarray*" : "std::vector<std::vector<long>>",
    "grib_vsarray*" : "std::vector<std::string>",
}

grib_array_function_mappings = {
    "grib_darray"  : "std::vector<double>",
    "grib_iarray"  : "std::vector<long>",
    "grib_sarray"  : "std::string",
    "grib_vdarray" : "std::vector<std::vector<double>>",
    "grib_viarray" : "std::vector<std::vector<long>>",
    "grib_vsarray" : "std::vector<std::string>",
}

# These will be used if no other supplied...
common_type_mappings = {
    "char**"            : "std::string&",
    "char*"             : "std::string",
    "char[]"            : "std::string",
    "FILE*"             : "FILE*",
}

common_function_arg_type_mappings = {
    "char*"   : "std::string&",
    "char[]"  : "std::string&",
}

def add_type_mappings_to_conversion_data(conversion_data):
    for type_mappings in [common_grib_type_mappings, grib_array_type_mappings, common_type_mappings]:
        for key, value in type_mappings.items():
            cdecl_spec = DeclSpec.from_decl_specifier_seq(key)
            cppdecl_spec = DeclSpec.from_decl_specifier_seq(value)
            conversion_data.add_funcbody_type_mapping(cdecl_spec, cppdecl_spec)

    for type_mappings in [common_function_arg_type_mappings]:
        for key, value in type_mappings.items():
            cdecl_spec = DeclSpec.from_decl_specifier_seq(key)
            cppdecl_spec = DeclSpec.from_decl_specifier_seq(value)
            conversion_data.add_funcsig_type_mapping(cdecl_spec, cppdecl_spec)
