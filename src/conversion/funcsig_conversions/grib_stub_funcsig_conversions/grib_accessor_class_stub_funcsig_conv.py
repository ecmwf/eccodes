
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_accessor_class_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_get_block_length", [Arg("grib_section*", "s"), Arg("size_t*", "l")]),
                    FuncSig("GribStatus", "gribGetBlockLength", [Arg("AccessorPtr", "ptr"), Arg("size_t&", "l")])),

    FuncSigMapping( FuncSig("grib_accessor*", "grib_accessor_factory", [Arg("grib_action*", "creator"), Arg("const long", "len"), Arg("grib_arguments*", "params")]),
                    FuncSig("AccessorPtr", "gribAccessorFactory", [Arg("GribActionPtr", "creator"), Arg("const long", "len"), Arg("AccessorInitData", "initData")])),

    FuncSigMapping( FuncSig("void", "grib_push_accessor", [Arg("grib_accessor*", "a"), Arg("grib_block_of_accessors*", "l")]),
                    FuncSig("void", "gribPushAccessor", [Arg("AccessorPtr", "ptr"), Arg("std::vector<AccessorPtr>", "ptrList")])),

    FuncSigMapping( FuncSig("int", "grib_section_adjust_sizes", [Arg("grib_section*", "s"), Arg("int", "update"), Arg("int", "depth")]),
                    FuncSig("GribStatus", "gribSectionAdjustSizes", [None, Arg("int", "update"), Arg("int", "depth")])),
]
