
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_buffer_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("void", "grib_buffer_delete", [Arg("const grib_context*", "c"), Arg("grib_buffer*", "b")]),
                    FuncSig("void", "griBufferDelete", [NONE_VALUE, Arg("AccessorDataBuffer&", "b")])),

    FuncSigMapping( FuncSig("void", "grib_buffer_replace", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", "data"), Arg("size_t", "newsize"), Arg("int", "update_lengths"), Arg("int", "update_paddings")]),
                    FuncSig("void", "gribBufferReplace", [NONE_VALUE, Arg("const AccessorDataBuffer&", "data"), Arg("size_t", "newsize"), Arg("int", "updateLengths"), Arg("int", "updatePaddings")])),

    FuncSigMapping( FuncSig("void", "grib_buffer_set_ulength_bits", [Arg("const grib_context*", "c"), Arg("grib_buffer*", "b"), Arg("size_t", "length_bits")]),
                    FuncSig("void", "gribBufferSetUlengthBits", [NONE_VALUE, Arg("AccessorDataBuffer&", "b"), Arg("size_t", "lengthBits")])),

    FuncSigMapping( FuncSig("grib_buffer*", "grib_create_growable_buffer", [Arg("const grib_context*", "c")]),
                    FuncSig("AccessorDataBuffer", "gribCreateGrowableBuffer", [NONE_VALUE])),

]
