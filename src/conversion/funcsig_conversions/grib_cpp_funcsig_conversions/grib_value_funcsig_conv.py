
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

    FuncSigMapping( FuncSig("int", "grib_get_array<T>", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetArray", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<T>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_array_internal<T>", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetArray", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<T>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetDouble", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<double>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetDouble", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<double>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetLong", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<long>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetLong", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<long>&", "value")])),


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

    FuncSigMapping( FuncSig("int", "grib_set_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetArray", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<T> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetArray", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<T> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetDouble", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<double> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetDouble", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<double> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetLong", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<long> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetLong", [None, Arg("AccessorName const&", "name"),  Arg("std::vector<long> const&", "value")])),
]
