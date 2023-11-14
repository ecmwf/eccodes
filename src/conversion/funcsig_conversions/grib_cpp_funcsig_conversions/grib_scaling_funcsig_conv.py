
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_scaling_funcsig_conversions = [
    FuncSigMapping( FuncSig("T", "codes_power<T>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<T>", [Arg("long", "s"), Arg("long", "n")])),

    FuncSigMapping( FuncSig("T", "codes_power<float>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<float>", [Arg("long", "s"), Arg("long", "n")])),

    FuncSigMapping( FuncSig("T", "codes_power<double>", [Arg("long", "s"), Arg("long", "n")]),
                    FuncSig("T", "codesPower<double>", [Arg("long", "s"), Arg("long", "n")])),
]
