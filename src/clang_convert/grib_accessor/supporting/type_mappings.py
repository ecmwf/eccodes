
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
import code_object_converter.conversion_pack.conversion_data as conversion_data
from code_object.code_interface import NONE_VALUE

# ==================== FUNCSIG TYPE MAPPINGS: Begin ====================

common_funcsig_type_mappings = {
    "char*"   : "std::string&",
    "char[]"  : "std::string&",
}

def add_funcsig_type_mappings_to_conversion_data(conversion_data):
    for type_mappings in [common_funcsig_type_mappings]:
        for ctype, cpptype in type_mappings.items():
            cdecl_spec = DeclSpec.from_decl_specifier_seq(ctype)
            cppdecl_spec = DeclSpec.from_decl_specifier_seq(cpptype)
            conversion_data.add_funcsig_type_mapping(cdecl_spec, cppdecl_spec)

# ==================== FUNCSIG TYPE MAPPINGS: End   ====================

# ==================== FUNCBODY TYPE MAPPINGS: Begin ====================

common_grib_funcbody_type_mappings = {
    "grib_accessor*"        : "AccessorPtr",
    "grib_accessor_class*"  : NONE_VALUE,
    "grib_handle*"          : NONE_VALUE,
    "grib_context*"         : NONE_VALUE,
    "grib_expression*"      : "GribExpressionPtr",
    "grib_codetable*"       : "GribCodeTablePtr",
    "grib_action*"          : "GribActionPtr"
}

grib_array_funcbody_type_mappings = {
    "grib_darray*"  : "std::vector<double>",
    "grib_iarray*"  : "std::vector<long>",
    "grib_sarray*"  : "std::string",
    "grib_vdarray*" : "std::vector<std::vector<double>>",
    "grib_viarray*" : "std::vector<std::vector<long>>",
    "grib_vsarray*" : "std::vector<std::string>",
}

# These will be used if no other supplied...
common_funcbody_type_mappings = {
    "char**"            : "std::string&",
    "char*"             : "std::string",
    "char[]"            : "std::string",
    "FILE*"             : "FILE*",
}

def add_funcbody_type_mappings_to_conversion_data(conversion_data):
    for type_mappings in [common_grib_funcbody_type_mappings, grib_array_funcbody_type_mappings, common_funcbody_type_mappings]:
        for ctype, cpptype in type_mappings.items():
            cdecl_spec = DeclSpec.from_decl_specifier_seq(ctype)
            cppdecl_spec = DeclSpec.from_decl_specifier_seq(cpptype)
            conversion_data.add_funcbody_type_mapping(cdecl_spec, cppdecl_spec)

# ==================== FUNCBODY TYPE MAPPINGS: End   ====================

def add_type_mappings_to_conversion_data(conversion_data):
    add_funcsig_type_mappings_to_conversion_data(conversion_data)
    add_funcbody_type_mappings_to_conversion_data(conversion_data)
