
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_value_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_get_size", [Arg("const grib_handle*", "ch"), Arg("const char*", "name"), Arg("size_t*", "size")]),
                    FuncSig("GribStatus", "gribGetSize", [None, Arg("AccessorName const&", "name"),  Arg("size_t&", "size")])),

    FuncSigMapping( FuncSig("int", "grib_get_double", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDouble", [None, Arg("AccessorName const&", "name"),  Arg("double&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDouble", [None, Arg("AccessorName const&", "name"),  Arg("double&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribGetLong", [None, Arg("AccessorName const&", "name"),  Arg("long&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribGetLong", [None, Arg("AccessorName const&", "name"),  Arg("long&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_string", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetString", [None, Arg("AccessorName const&", "name"),  Arg("std::string&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_string_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetString", [None, Arg("AccessorName const&", "name"),  Arg("std::string&", "value")])),



    FuncSigMapping( FuncSig("int", "grib_set_double", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribSetDouble", [None, Arg("AccessorName const&", "name"),  Arg("double", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribSetDouble", [None, Arg("AccessorName const&", "name"),  Arg("double", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribSetLong", [None, Arg("AccessorName const&", "name"),  Arg("long", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribSetLong", [None, Arg("AccessorName const&", "name"),  Arg("long", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_string", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetString", [None, Arg("AccessorName const&", "name"),  Arg("std::string", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_string_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetString", [None, Arg("AccessorName const&", "name"),  Arg("std::string", "value")])),
]
