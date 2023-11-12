
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_context_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("char*", "grib_context_full_defs_path", [Arg("grib_context*", "c"), Arg("const char*", "basename")]),
                    FuncSig("std::string", "gribContextFullDefsPath", [None, Arg("std::string", "basename")])),

]
