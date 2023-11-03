
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_query_funcsig_conversions = [
    FuncSigMapping( FuncSig("grib_accessor*", "grib_find_accessor", [Arg("const grib_handle*", "h"), Arg("const char*", "name")]),
                    FuncSig("AccessorPtr", "gribFindAccessor", [None, Arg("AccessorName const&", "name")])),

]
