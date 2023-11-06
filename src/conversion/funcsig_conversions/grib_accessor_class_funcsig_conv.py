
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_accessor_class_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_get_block_length", [Arg("grib_section*", "s"), Arg("size_t*", "l")]),
                    FuncSig("GribStatus", "gribGetBlockLength", [Arg("AccessorPtr", "ptr"), Arg("size_t&", "l")])),
]
