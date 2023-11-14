
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_context_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("char*", "grib_context_full_defs_path", [Arg("grib_context*", "c"), Arg("const char*", "basename")]),
                    FuncSig("std::string", "gribContextFullDefsPath", [None, Arg("std::string", "basename")])),

    FuncSigMapping( FuncSig("int", "grib_context_get_handle_file_count", [Arg("grib_context*", "c")]),
                    FuncSig("int", "gribContextGetHandleFileCount", [None])),

    FuncSigMapping( FuncSig("int", "grib_context_get_handle_total_count", [Arg("grib_context*", "c")]),
                    FuncSig("int", "gribContextGetHandleTotalCount", [None])),

    FuncSigMapping( FuncSig("void*", "grib_context_buffer_malloc_clear", [Arg("grib_context*", "c"), Arg("size_t", "size")]),
                    FuncSig("AccessorDataBuffer", "gribcontextBufferMallocClear", [None, Arg("size_t", "size")])),

]
