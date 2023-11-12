
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_parse_utils_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_recompose_name", [Arg("grib_handle*", "h"), Arg("grib_accessor*", "observer"), Arg("const char*", "uname"), Arg("char*", "fname"), Arg("int", "fail")]),
                    FuncSig("GribStatus", "gribRecomposeName", [None, Arg("AccessorPtr", "observer"), Arg("const std::string", "uname"), Arg("std::string", "fname"), Arg("int", "fail")])),

]
