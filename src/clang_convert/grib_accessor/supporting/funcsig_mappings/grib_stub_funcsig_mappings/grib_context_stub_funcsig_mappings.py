
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_context_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("char*", "grib_context_full_defs_path", [Arg("grib_context*", "c"), Arg("const char*", "basename")]),
                    FuncSig("std::string", "gribContextFullDefsPath", [NONE_VALUE, Arg("std::string", "basename")])),

    FuncSigMapping( FuncSig("int", "grib_context_get_handle_file_count", [Arg("grib_context*", "c")]),
                    FuncSig("int", "gribContextGetHandleFileCount", [NONE_VALUE])),

    FuncSigMapping( FuncSig("int", "grib_context_get_handle_total_count", [Arg("grib_context*", "c")]),
                    FuncSig("int", "gribContextGetHandleTotalCount", [NONE_VALUE])),

    FuncSigMapping( FuncSig("void*", "grib_context_buffer_malloc_clear", [Arg("grib_context*", "c"), Arg("size_t", "size")]),
                    FuncSig("AccessorDataBuffer", "gribcontextBufferMallocClear", [NONE_VALUE, Arg("size_t", "size")])),

]
