
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

string_util_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "string_to_long", [Arg("const char*", "input"), Arg("long*", "output"), Arg("int", "strict")]),
                    FuncSig("int", "stringToLong", [Arg("std::string", "input"), Arg("long&", "output"), Arg("int", "strict")])),

]
