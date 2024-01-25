
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping

# Conversions for common Grib functions

grib_common_funcsig_mappings = [
    # Define a 1:1 mapping for now as this is processed in other places
    FuncSigMapping( FuncSig("grib_handle*", "grib_handle_of_accessor", [Arg("const grib_accessor*", "a")]),
                    FuncSig("grib_handle*", "grib_handle_of_accessor", [Arg("const grib_accessor*", "a")])),

]
