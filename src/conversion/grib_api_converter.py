# Entry point for all GRIB API conversions 
import re
import grib_api.grib_array as grib_array
import grib_api.grib_funcsig_type_transforms as grib_funcsig_type_transforms
import grib_api.grib_type_transforms as grib_type_transforms
import grib_api.grib_values as grib_values

def grib_api_funcsig_type_transforms():
    funcsig_type_transforms =  grib_funcsig_type_transforms.all_grib_funcsig_type_transforms()
    return funcsig_type_transforms

def grib_api_type_transforms():
    type_transforms =  grib_type_transforms.all_grib_type_transforms()
    return type_transforms

def convert_grib_api_functions(line):
    line = grib_array.convert_grib_array_functions(line)
    return line

def convert_grib_api_definitions(line):
    line = grib_values.convert_grib_values(line)
    return line

def process_grib_api_variables(line, arg_transforms):
    for carg, cpparg in arg_transforms.items():
        if re.match(r"grib_v?[dis]array", carg.underlying_type):
            line = grib_array.process_grib_array_variables(line, carg, cpparg)

    return line