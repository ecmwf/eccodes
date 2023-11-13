
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_value_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_get_double_element", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("int", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDoubleElement", [None, Arg("AccessorName const&", "name"), Arg("int", "i"), Arg("double&", "val")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("int", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDoubleElement", [None, Arg("AccessorName const&", "name"), Arg("int", "i"), Arg("double&", "val")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_set", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "gribGetDoubleElementSet", [None, Arg("AccessorName const&", "name"), Arg("const std::vector<size_t>*", "indexArray"), Arg("size_t", "len"), Arg("std::vector<double>&", "valArray")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_set_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "gribGetDoubleElementSet", [None, Arg("AccessorName const&", "name"), Arg("const std::vector<size_t>*", "indexArray"), Arg("size_t", "len"), Arg("std::vector<double>&", "valArray")])),
]

