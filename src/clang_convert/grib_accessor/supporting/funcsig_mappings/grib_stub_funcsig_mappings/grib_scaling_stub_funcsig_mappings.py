
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping

grib_scaling_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("long", "grib_get_binary_scale_fact", [Arg("double", "max"), Arg("double", "min"), Arg("long", "bpval"), Arg("int*", "error")]),
                    FuncSig("long", "gribGetBinaryScaleFact", [Arg("double", "max"), Arg("double", "min"), Arg("long", "bpval"), Arg("GribStatus&", "error")])),

]

