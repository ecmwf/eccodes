
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping

grib_bits_funcsig_mappings = [
    FuncSigMapping( FuncSig("void", "grib_set_bit_on", [Arg("unsigned char*", "p"), Arg("long*", "bitp")]),
                    FuncSig("void", "gribSetBitOn", [Arg("AccessorDataPointer", "p"), Arg("long&", "bitp")])),

    FuncSigMapping( FuncSig("void", "grib_set_bits_on", [Arg("unsigned char*", "p"), Arg("long*", "bitp"), Arg("long", "nbits")]),
                    FuncSig("void", "gribSetBitsOn", [Arg("AccessorDataPointer", "p"), Arg("long&", "bitp"), Arg("long", "nbits")])),

    FuncSigMapping( FuncSig("void", "grib_set_bit_off", [Arg("unsigned char*", "p"), Arg("long*", "bitp")]),
                    FuncSig("void", "gribSetBitOff", [Arg("AccessorDataPointer", "p"), Arg("long&", "bitp")])),

    FuncSigMapping( FuncSig("void", "grib_set_bit", [Arg("unsigned char*", "p"), Arg("long", "bitp"), Arg("int", "val")]),
                    FuncSig("void", "gribSetBit", [Arg("AccessorDataPointer", "p"), Arg("long&", "bitp"), Arg("int", "val")])),

]
