
# Note: grib_accessor* different to funcisg!
common_grib_type_transforms = {
    "grib_accessor*"    : "AccessorName",
    "grib_handle*"      : None,
    "grib_context*"     : None,
}

grib_array_type_transforms = {
    "grib_darray"  : "std::vector<double>",
    "grib_iarray"  : "std::vector<long>",
    "grib_sarray"  : "std::string",
    "grib_vdarray" : "std::vector<std::vector<double>>",
    "grib_viarray" : "std::vector<std::vector<long>>",
    "grib_vsarray" : "std::vector<std::string>",
}

def all_grib_type_transforms():

    type_transforms_list = [
        common_grib_type_transforms,
        grib_array_type_transforms,
    ]

    type_transforms = {}

    for transforms_list in type_transforms_list:
        type_transforms.update(transforms_list)

    return type_transforms