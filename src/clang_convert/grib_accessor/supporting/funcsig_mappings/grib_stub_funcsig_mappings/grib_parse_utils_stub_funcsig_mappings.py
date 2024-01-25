
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_parse_utils_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_recompose_name", [Arg("grib_handle*", "h"), Arg("grib_accessor*", "observer"), Arg("const char*", "uname"), Arg("char*", "fname"), Arg("int", "fail")]),
                    FuncSig("GribStatus", "gribRecomposeName", [NONE_VALUE, Arg("AccessorPtr", "observer"), Arg("const std::string", "uname"), Arg("std::string", "fname"), Arg("int", "fail")])),

]
