
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping

grib_scaling_funcsig_mappings = [
    FuncSigMapping( FuncSig("T", "codes_power<T>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<T>", [Arg("long", "s"), Arg("long", "n")])),

    FuncSigMapping( FuncSig("T", "codes_power<float>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<float>", [Arg("long", "s"), Arg("long", "n")])),

    FuncSigMapping( FuncSig("T", "codes_power<double>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<double>", [Arg("long", "s"), Arg("long", "n")])),
]
