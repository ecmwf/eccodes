
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_bits_fast_big_endian_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_encode_unsigned_longb", [Arg("const unsigned char*", "p"), Arg("unsigned long", "val"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("GribStatus", "gribEncodeUnsignedLongb", [Arg("AccessorDataBuffer::const_pointer", "input"), Arg("unsigned long", "val"), Arg("long&", "bitPos"), Arg("long", "numBits")])),
]
