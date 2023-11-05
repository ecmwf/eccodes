
import funcsig_conversions.grib_common_funcsig_conv as grib_common_funcsig_conv
import funcsig_conversions.grib_query_funcsig_conv as grib_query_funcsig_conv
import funcsig_conversions.grib_value_funcsig_conv as grib_value_funcsig_conv
import funcsig_conversions.string_util_funcsig_conv as string_util_funcsig_conv

funcsig_conversions = [
    grib_common_funcsig_conv.grib_common_funcsig_conversions,
    grib_query_funcsig_conv.grib_query_funcsig_conversions,
    grib_value_funcsig_conv.grib_value_funcsig_conversions,
    string_util_funcsig_conv.string_util_funcsig_conversions
]

# Return a list of all the funcsig conversions
def all_funcsig_conversions():
    return funcsig_conversions
