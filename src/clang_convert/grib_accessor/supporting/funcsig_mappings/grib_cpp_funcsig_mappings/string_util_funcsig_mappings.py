
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping

string_util_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "string_to_long", [Arg("const char*", "input"), Arg("long*", "output"), Arg("int", "strict")]),
                    FuncSig("int", "stringToLong", [Arg("std::string", "input"), Arg("long&", "output"), Arg("int", "strict")])),

]
