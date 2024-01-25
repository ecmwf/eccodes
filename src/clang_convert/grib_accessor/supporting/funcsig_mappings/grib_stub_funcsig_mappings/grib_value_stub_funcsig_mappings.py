
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_value_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_get_double_element", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("int", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDoubleElement", [NONE_VALUE, Arg("AccessorName const&", "name"), Arg("int", "i"), Arg("double&", "val")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("int", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "gribGetDoubleElement", [NONE_VALUE, Arg("AccessorName const&", "name"), Arg("int", "i"), Arg("double&", "val")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_set", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "gribGetDoubleElementSet", [NONE_VALUE, Arg("AccessorName const&", "name"), Arg("const std::vector<size_t>*", "indexArray"), Arg("size_t", "len"), Arg("std::vector<double>&", "valArray")])),

    FuncSigMapping( FuncSig("int", "grib_get_double_element_set_internal", [Arg("const grib_handle*", "h"), Arg("const char*", "name"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "gribGetDoubleElementSet", [NONE_VALUE, Arg("AccessorName const&", "name"), Arg("const std::vector<size_t>*", "indexArray"), Arg("size_t", "len"), Arg("std::vector<double>&", "valArray")])),

    FuncSigMapping( FuncSig("int", "grib_get_nearest_smaller_value", [Arg("grib_handle*", "h"), Arg("const char*", "name"), Arg("double", "val"), Arg("double*", "nearest")]),
                    FuncSig("GribStatus", "gribGetNearestSmallerValue", [NONE_VALUE, Arg("AccessorName const&", "name"), Arg("double", "val"), Arg("double&", "nearest")])),
]

