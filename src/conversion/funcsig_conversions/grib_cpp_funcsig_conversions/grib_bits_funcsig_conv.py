
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_bits_funcsig_conversions = [
    FuncSigMapping( FuncSig("void", "grib_set_bit_on", [Arg("unsigned char*", "p"), Arg("long*", "bitp")]),
                    FuncSig("void", "gribSetBitOn", [Arg("std::vector<unsigned char>&", "p"), Arg("long&", "bitp")])),

    FuncSigMapping( FuncSig("void", "grib_set_bits_on", [Arg("unsigned char*", "p"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("void", "gribSetBitsOn", [Arg("std::vector<unsigned char>&", "p"), Arg("long&", "bitp"), Arg("long", "nbits")])),

    FuncSigMapping( FuncSig("void", "grib_set_bit_off", [Arg("unsigned char*", "p"), Arg("long*", "bitp")]),
                    FuncSig("void", "gribSetBitOff", [Arg("std::vector<unsigned char>&", "p"), Arg("long&", "bitp")])),

    FuncSigMapping( FuncSig("void", "grib_set_bit", [Arg("unsigned char*", "p"), Arg("long", "bitp"), Arg("int", "val")]),
                    FuncSig("void", "gribSetBit", [Arg("std::vector<unsigned char>&", "p"), Arg("long&", "bitp"), Arg("int", "val")])),

]
