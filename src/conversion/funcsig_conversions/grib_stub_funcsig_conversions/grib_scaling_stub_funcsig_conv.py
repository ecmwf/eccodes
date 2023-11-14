
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_scaling_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("long", "grib_get_binary_scale_fact", [Arg("double", "max"), Arg("double", "min"), Arg("long", "bpval"), Arg("int*", "error")]),
                    FuncSig("long", "gribGetBinaryScaleFact", [Arg("double", "max"), Arg("double", "min"), Arg("long", "bpval"), Arg("GribStatus&", "error")])),

]

