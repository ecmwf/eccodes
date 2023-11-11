
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_bits_any_endian_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("unsigned long", "grib_decode_unsigned_long", [Arg("const unsigned char*", "p"), Arg("long*", "p"), Arg("long", "nbits")]),
                    FuncSig("unsigned long", "gribDecodeUnsignedLong", [Arg("AccessorDataBuffer::const_pointer", "input"), Arg("long&", "bitPos"), Arg("long", "numBits")])),
]
