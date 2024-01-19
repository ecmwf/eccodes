
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.funcsig import FuncSig
from code_object.arg import Arg
from code_object_converter.supporting.arg_indexes import ArgIndexes

# NOTE: The C FuncSig variable names may be different so should only be used as a guide (some are missing)
grib_accessor_member_funcsig_mapping = [
    
    # Constructor: static void init(grib_accessor*, const long, grib_arguments*);
    FuncSigMapping( FuncSig("", "init", [Arg("grib_context*", ""), Arg("const long", ""), Arg("grib_arguments*", "")]),
                    FuncSig("", "Constructor",   [None, None, Arg("AccessorInitData const&", "initData")]),
                    ArgIndexes(cbuffer=2, clength=1, cpp_container=2)),

    # Destructor: static void destroy(grib_context*, grib_accessor*);
    FuncSigMapping( FuncSig("", "destroy", [Arg("grib_context*", ""), Arg("grib_accessor*", "")]),
                    FuncSig("", "Destructor", [None, None]) ),
]
