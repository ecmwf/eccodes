
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
import code_object_converter.conversion_pack.conversion_data as conversion_data

# These will be used if no other supplied...
common_funcbody_arg_mappings = {
    Arg("int", "err")   : Arg("GribStatus", "err"),
    Arg("int", "ret")   : Arg("GribStatus", "ret"),
}

def add_arg_mappings_to_conversion_data(conversion_data):
    for funcbody_arg_mappings in [common_funcbody_arg_mappings]:
        for key, value in funcbody_arg_mappings.items():
            conversion_data.add_funcbody_arg_mapping(key, value)
