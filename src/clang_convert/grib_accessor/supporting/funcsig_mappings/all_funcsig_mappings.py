
import grib_accessor.supporting.funcsig_mappings.grib_cpp_funcsig_mappings.all_grib_cpp_funcsig_mappings as all_grib_cpp_funcsig_mappings
import grib_accessor.supporting.funcsig_mappings.grib_stub_funcsig_mappings.all_grib_stub_funcsig_mappings as all_grib_stub_funcsig_mappings
from code_object.declaration_specifier import DeclSpec
import utils.debug as debug

# Return a list of all the funcsig conversions
def all_funcsig_mappings():
    return all_grib_cpp_funcsig_mappings.all_grib_cpp_funcsig_mappings() + all_grib_stub_funcsig_mappings.all_grib_stub_funcsig_mappings()

def add_all_funcsig_mappings_to_conversion_data(conversion_data):
    for mapping in all_funcsig_mappings():
        for entry in mapping:
            debug.line("add_all_funcsig_mappings_to_conversion_data", f"ADDING FUNCIG MAPPING: [{entry.cfuncsig.name}] -> [{entry.cppfuncsig.name}]")
            conversion_data.add_funcsig_mapping(entry)