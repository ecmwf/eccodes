
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_bits_any_endian_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("unsigned long", "grib_decode_unsigned_long", [Arg("const unsigned char*", "p"), Arg("long*", "p"), Arg("long", "nbits")]),
                    FuncSig("unsigned long", "gribDecodeUnsignedLong", [Arg("const AccessorDataPointer", "input"), Arg("long&", "bitPos"), Arg("long", "numBits")])),

    FuncSigMapping( FuncSig("int", "grib_encode_unsigned_long", [Arg("unsigned char*", "p"), Arg("unsigned long", "val"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("GribStatus", "gribEncodeUnsignedLong", [Arg("AccessorDataPointer", "input"), Arg("unsigned long", "val"), Arg("long&", "bitPos"), Arg("long", "numBits")])),

    FuncSigMapping( FuncSig("int", "grib_encode_unsigned_longb", [Arg("unsigned char*", "p"), Arg("unsigned long", "val"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("GribStatus", "gribEncodeUnsignedLongb", [Arg("AccessorDataPointer", "input"), Arg("unsigned long", "val"), Arg("long&", "bitPos"), Arg("long", "numBits")])),
]
