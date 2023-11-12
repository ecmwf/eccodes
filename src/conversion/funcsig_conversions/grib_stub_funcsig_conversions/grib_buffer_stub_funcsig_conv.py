
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_buffer_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("void", "grib_buffer_delete", [Arg("const grib_context*", "c"), Arg("grib_buffer*", "b")]),
                    FuncSig("void", "griBufferDelete", [None, Arg("AccessorDataBuffer", "b")])),

    FuncSigMapping( FuncSig("void", "grib_buffer_replace", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", "data"), Arg("size_t", "newsize"), Arg("int", "update_lengths"), Arg("int", "update_paddings")]),
                    FuncSig("void", "gribBufferReplace", [Arg("AccessorPtr", "ptr"), Arg("AccessorDataBuffer", "data"), Arg("size_t", "newsize"), Arg("int", "updateLengths"), Arg("int", "updatePaddings")])),

    FuncSigMapping( FuncSig("void", "grib_buffer_set_ulength_bits", [Arg("const grib_context*", "c"), Arg("grib_buffer*", "b"), Arg("size_t", "length_bits")]),
                    FuncSig("void", "gribBufferSetUlengthBits", [None, Arg("AccessorDataBuffer", "b"), Arg("size_t", "lengthBits")])),

    FuncSigMapping( FuncSig("grib_buffer*", "grib_create_growable_buffer", [Arg("const grib_context*", "c")]),
                    FuncSig("AccessorDataBuffer", "gribCreateGrowableBuffer", [None])),

]
