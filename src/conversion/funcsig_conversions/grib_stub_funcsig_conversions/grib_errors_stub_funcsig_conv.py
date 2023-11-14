
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_errors_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("const char*", "grib_get_error_message", [Arg("int", "code")]),
                    FuncSig("std::string", "gribGetErrorMessage", [Arg("GribStatus", "code")])),

]

