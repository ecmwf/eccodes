# Provides a way to differentiate funcsig types from function body types
# For example grib_accessor* is None here, but set to "AccessorName" in the body...
#
# Note: Any types that have the same transform in the body are removed here
#       as the body list is always processed after the funsig list anyway...
common_grib_funcsig_type_transforms = {
    "grib_accessor*[]"  : "std::vector<AccessorName>",
    "grib_accessor*"    : None,
}

def all_grib_funcsig_type_transforms():

    funcsig_type_transforms_list = [
        common_grib_funcsig_type_transforms,
    ]

    funcsig_type_transforms = {}

    for funcsig_transforms_list in funcsig_type_transforms_list:
        funcsig_type_transforms.update(funcsig_transforms_list)

    return funcsig_type_transforms
