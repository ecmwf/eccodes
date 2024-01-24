
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping

grib_errors_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("const char*", "grib_get_error_message", [Arg("int", "code")]),
                    FuncSig("std::string", "gribGetErrorMessage", [Arg("GribStatus", "code")])),

]

