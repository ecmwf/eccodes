
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

# Conversions for common Grib functions

grib_common_funcsig_conversions = [
    # Define a 1:1 mapping for now as this is processed in other places
    FuncSigMapping( FuncSig("grib_handle*", "grib_handle_of_accessor", [Arg("const grib_accessor*", "a")]),
                    FuncSig("grib_handle*", "grib_handle_of_accessor", [Arg("const grib_accessor*", "a")])),

]
