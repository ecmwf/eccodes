
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_ieeefloat_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_ieee_decode_array<T>", [Arg("grib_context*", "c"), Arg("unsigned char*", "buf"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("T*", "val")]),
                    FuncSig("GribStatus", "gribIeeeDecodeArray", [NONE_VALUE, Arg("AccessorDataPointer", "buf"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("std::vector<T>", "val")])),

    FuncSigMapping( FuncSig("int", "grib_ieee_encode_array", [Arg("grib_context*", "c"), Arg("double*", "val"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("unsigned char*", "buf")]),
                    FuncSig("GribStatus", "gribIeeeEncodeArray", [NONE_VALUE, Arg("std::vector<double>&", "val"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("AccessorDataBuffer&", "buf")])),

    FuncSigMapping( FuncSig("unsigned long", "grib_ieee_to_long", [Arg("double", "x")]),
                    FuncSig("unsigned long", "gribIeeeToLong", [Arg("double", "x")])),

    FuncSigMapping( FuncSig("unsigned long", "grib_ieee64_to_long", [Arg("double", "x")]),
                    FuncSig("unsigned long", "gribIeee64ToLong", [Arg("double", "x")])),

]
