# Entry point for all GRIB API conversions 

import grib_api.grib_array as grib_array

def convert_grib_api_functions(line):
    line = grib_array.convert_grib_array_functions(line)
    return line