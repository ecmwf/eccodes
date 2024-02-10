
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
import code_object_converter.conversion_pack.conversion_data as conversion_data
from code_object.code_interface import NONE_VALUE

# ==================== FUNCSIG ARG MAPPINGS: Begin ====================

common_funcsig_arg_mappings = {
    Arg("grib_accessor*", "a")   : NONE_VALUE,
}

def add_funcsig_arg_mappings_to_conversion_data(conversion_data):
    for funcsig_arg_mappings in [common_funcsig_arg_mappings]:
        for carg, cpparg in funcsig_arg_mappings.items():
            conversion_data.add_funcsig_arg_mapping(carg, cpparg)

# ==================== FUNCSIG ARG MAPPINGS: End   ====================

# ==================== FUNCBODY ARG MAPPINGS: Begin ====================

common_funcbody_arg_mappings = {
    Arg("int", "err")   : Arg("GribStatus", "err"),
    Arg("int", "ret")   : Arg("GribStatus", "ret"),
}

def add_funcbody_arg_mappings_to_conversion_data(conversion_data):
    for funcbody_arg_mappings in [common_funcbody_arg_mappings]:
        for carg, cpparg in funcbody_arg_mappings.items():
            conversion_data.add_funcbody_arg_mapping(carg, cpparg)

# ==================== FUNCBODY ARG MAPPINGS: End   ====================

def add_arg_mappings_to_conversion_data(conversion_data):
    add_funcsig_arg_mappings_to_conversion_data(conversion_data)
    add_funcbody_arg_mappings_to_conversion_data(conversion_data)
