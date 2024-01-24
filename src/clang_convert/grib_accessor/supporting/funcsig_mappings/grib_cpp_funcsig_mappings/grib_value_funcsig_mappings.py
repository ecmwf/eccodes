
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_value_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_get_size", [Arg("const grib_handle*", "ch"), Arg("const char*", "name"), Arg("size_t*", "size")]),
                    FuncSig("GribStatus", "gribGetSize", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("size_t&", "size")])),

    FuncSigMapping( FuncSig("int", "grib_get_double", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("double&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("double&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribGetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("long&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribGetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("long&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_string", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetString", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::string&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_string_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetString", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::string&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_array<T>", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetArray", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<T>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_array_internal<T>", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetArray", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<T>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<double>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<double>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<long>&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_get_long_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribGetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<long>&", "value")])),


    FuncSigMapping( FuncSig("int", "grib_set_double", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribSetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("double", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribSetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("double", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribSetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("long", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribSetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("long", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_string", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetString", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::string", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_string_internal", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("char*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetString", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::string", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetArray", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<T> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("T*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetArray", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<T> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<double> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_double_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("double*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetDouble", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<double> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_array", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<long> const&", "value")])),

    FuncSigMapping( FuncSig("int", "grib_set_long_array_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("long*", "val"), Arg("size_t*", "length")]),
                    FuncSig("GribStatus", "gribSetLong", [NONE_VALUE, Arg("AccessorName const&", "name"),  Arg("std::vector<long> const&", "value")])),
]
