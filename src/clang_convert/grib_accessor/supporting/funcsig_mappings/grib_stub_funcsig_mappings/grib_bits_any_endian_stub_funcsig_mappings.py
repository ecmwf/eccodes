
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping

grib_bits_any_endian_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("unsigned long", "grib_decode_unsigned_long", [Arg("const unsigned char*", "p"), Arg("long*", "p"), Arg("long", "nbits")]),
                    FuncSig("unsigned long", "gribDecodeUnsignedLong", [Arg("const AccessorDataPointer", "input"), Arg("long&", "bitPos"), Arg("long", "numBits")])),

    FuncSigMapping( FuncSig("int", "grib_encode_unsigned_long", [Arg("unsigned char*", "p"), Arg("unsigned long", "val"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("GribStatus", "gribEncodeUnsignedLong", [Arg("AccessorDataPointer", "input"), Arg("unsigned long", "val"), Arg("long&", "bitPos"), Arg("long", "numBits")])),

    FuncSigMapping( FuncSig("int", "grib_encode_unsigned_longb", [Arg("unsigned char*", "p"), Arg("unsigned long", "val"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("GribStatus", "gribEncodeUnsignedLongb", [Arg("AccessorDataPointer", "input"), Arg("unsigned long", "val"), Arg("long&", "bitPos"), Arg("long", "numBits")])),

    FuncSigMapping( FuncSig("int", "grib_decode_long_array", [Arg("const unsigned char*", "p"), Arg("long*", "bitp"), Arg("long", "bitsPerValue"), Arg("size_t", "n_vals"), Arg("long*", "val")]),
                    FuncSig("GribStatus", "gribDecodeLongArray", [Arg("const AccessorDataPointer", "input"), Arg("long&", "bitp"), Arg("long", "bitsPerValue"), Arg("size_t", "nVals"), Arg("std::vector<long>&", "val")])),
]
