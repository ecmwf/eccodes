
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_ieeefloat_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_ieee_decode_array<T>", [Arg("grib_context*", "c"), Arg("unsigned char*", "buf"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("T*", "val")]),
                    FuncSig("GribStatus", "gribIeeeDecodeArray", [None, Arg("AccessorDataPointer", "buf"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("std::vector<T>", "val")])),

    FuncSigMapping( FuncSig("int", "grib_ieee_encode_array", [Arg("grib_context*", "c"), Arg("double*", "val"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("unsigned char*", "buf")]),
                    FuncSig("GribStatus", "gribIeeeEncodeArray", [None, Arg("std::vector<double>&", "val"), Arg("size_t", "nvals"), Arg("int", "bytes"), Arg("AccessorDataBuffer&", "buf")])),

    FuncSigMapping( FuncSig("unsigned long", "grib_ieee_to_long", [Arg("double", "x")]),
                    FuncSig("unsigned long", "gribIeeeToLong", [Arg("double", "x")])),

    FuncSigMapping( FuncSig("unsigned long", "grib_ieee64_to_long", [Arg("double", "x")]),
                    FuncSig("unsigned long", "gribIeee64ToLong", [Arg("double", "x")])),

]
