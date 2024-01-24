
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_query_funcsig_mappings = [
    FuncSigMapping( FuncSig("grib_accessor*", "grib_find_accessor", [Arg("const grib_handle*", "h"), Arg("const char*", "name")]),
                    FuncSig("AccessorPtr", "gribFindAccessor", [NONE_VALUE, Arg("AccessorName const&", "name")])),

]
