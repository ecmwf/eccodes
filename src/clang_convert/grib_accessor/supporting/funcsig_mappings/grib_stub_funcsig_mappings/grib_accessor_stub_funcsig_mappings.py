
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_accessor_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_inline_strcmp", [Arg("const char*", "a"), Arg("const char*", "b")]),
                    FuncSig("GribStatus", "gribInlineStrcmp", [Arg("const std::string", "a"), Arg("const std::string", "b")])),

    FuncSigMapping( FuncSig("int", "grib_pack_missing", [Arg("grib_accessor*", "a")]),
                    FuncSig("GribStatus", "gribPackMissing", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("int", "grib_pack_zero", [Arg("grib_accessor*", "a")]),
                    FuncSig("GribStatus", "gribPackZero", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("int", "grib_is_missing_internal", [Arg("grib_accessor*", "a")]),
                    FuncSig("GribStatus", "gribIsMissingInternal", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("int", "grib_pack_double", [Arg("grib_accessor*", "a"), Arg("const double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackDouble", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<double>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_pack_float", [Arg("grib_accessor*", "a"), Arg("const float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackFloat", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<float>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_pack_expression", [Arg("grib_accessor*", "a"), Arg("grib_expression*", "e")]),
                    FuncSig("GribStatus", "gribPackExpression", [Arg("AccessorPtr", "ptr"), Arg("GribExpressionPtr", "e")])),

    FuncSigMapping( FuncSig("int", "grib_pack_string", [Arg("grib_accessor*", "a"), Arg("const char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackString", [Arg("AccessorPtr", "ptr"), Arg("const std::string", "value"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_pack_string_array", [Arg("grib_accessor*", "a"), Arg("const char**", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackStringArray", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<std::string>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_pack_long", [Arg("grib_accessor*", "a"), Arg("const long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackLong", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<long>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_pack_bytes", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribPackBytes", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<unsigned char>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # =============================================================================

    FuncSigMapping( FuncSig("int", "grib_unpack_bytes", [Arg("grib_accessor*", "a"), Arg("unsigned char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackBytes", [Arg("AccessorPtr", "ptr"), Arg("std::vector<unsigned char>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_double_subarray", [Arg("grib_accessor*", "a"), Arg("double*", "v"), Arg("size_t", "start"), Arg("size_t", "len")]),
                    FuncSig("GribStatus", "gribUnpackDoubleSubarray", [Arg("AccessorPtr", "ptr"), Arg("std::vector<double>&", "values"), Arg("size_t", "start"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=3, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_double", [Arg("grib_accessor*", "a"), Arg("double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackDouble", [Arg("AccessorPtr", "ptr"), Arg("std::vector<double>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_float", [Arg("grib_accessor*", "a"), Arg("float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackFloat", [Arg("AccessorPtr", "ptr"), Arg("std::vector<float>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_double_element", [Arg("grib_accessor*", "a"), Arg("size_t", "i"), Arg("double*", "v")]),
                    FuncSig("GribStatus", "gribUnpackDoubleElement", [Arg("AccessorPtr", "ptr"), Arg("size_t", "i"), Arg("std::vector<double>&", "values")])),

    FuncSigMapping( FuncSig("int", "grib_unpack_float_element", [Arg("grib_accessor*", "a"), Arg("size_t", "i"), Arg("float*", "v")]),
                    FuncSig("GribStatus", "gribUnpackFloatElement", [Arg("AccessorPtr", "ptr"), Arg("size_t", "i"), Arg("std::vector<float>&", "values")])),

    FuncSigMapping( FuncSig("int", "grib_unpack_double_element_set", [Arg("grib_accessor*", "a"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "gribUnpackDoubleElementSet", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<size_t>&", "index_array"), Arg("std::vector<double>&", "valArray"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_float_element_set", [Arg("grib_accessor*", "a"), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("float*", "val_array")]),
                    FuncSig("GribStatus", "gribUnpackFloatElementSet", [Arg("AccessorPtr", "ptr"), Arg("const std::vector<size_t>&", "index_array"), Arg("std::vector<float>&", "valArray"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_string", [Arg("grib_accessor*", "a"), Arg("char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackString", [Arg("AccessorPtr", "ptr"), Arg("std::string&", "value"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_string_array", [Arg("grib_accessor*", "a"), Arg("char**", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackStringArray", [Arg("AccessorPtr", "ptr"), Arg("std::vector<std::string>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_unpack_long", [Arg("grib_accessor*", "a"), Arg("long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "gribUnpackLong", [Arg("AccessorPtr", "ptr"), Arg("std::vector<long>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # =============================================================================

    FuncSigMapping( FuncSig("int", "grib_accessors_list_unpack_long", [Arg("grib_accessors_list*", "al"), Arg("long*", "v"), Arg("size_t*", "buffer_len")]),
                    FuncSig("GribStatus", "gribAccessorsListUnpackLong", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("std::vector<long>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_accessors_list_unpack_double", [Arg("grib_accessors_list*", "al"), Arg("double*", "v"), Arg("size_t*", "buffer_len")]),
                    FuncSig("GribStatus", "gribAccessorsListUnpackDouble", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("std::vector<double>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_accessors_list_unpack_float", [Arg("grib_accessors_list*", "al"), Arg("float*", "v"), Arg("size_t*", "buffer_len")]),
                    FuncSig("GribStatus", "gribAccessorsListUnpackFloat", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("std::vector<float>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_accessors_list_unpack_string", [Arg("grib_accessors_list*", "al"), Arg("char**", "val"), Arg("size_t*", "buffer_len")]),
                    FuncSig("GribStatus", "gribAccessorsListUnpackString", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("std::vector<std::string>&", "values"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping( FuncSig("int", "grib_accessors_list_value_count", [Arg("grib_accessors_list*", "al"), Arg("size_t*", "count")]),
                    FuncSig("GribStatus", "gribAccessorsListValueCount", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("size_t&", "count")])),

    FuncSigMapping( FuncSig("grib_accessors_list*", "grib_accessors_list_create", [Arg("grib_context*", "c")]),
                    FuncSig("std::vector<AccessorPtr>", "gribAccessorsListCreate", [NONE_VALUE])),

    FuncSigMapping( FuncSig("void", "grib_accessors_list_push", [Arg("grib_accessors_list*", "al"), Arg("int", "rank")]),
                    FuncSig("void", "gribAccessorsListPush", [Arg("std::vector<AccessorPtr>", "ptrList"), Arg("int", "rank")])),

    FuncSigMapping( FuncSig("void", "grib_accessors_list_delete", [Arg("grib_context*", "c"), Arg("grib_accessors_list*", "al")]),
                    FuncSig("void", "gribAccessorsListDelete", [NONE_VALUE, Arg("std::vector<AccessorPtr>", "ptrList")])),

    # =============================================================================

    FuncSigMapping( FuncSig("long", "grib_accessor_get_native_type", [Arg("grib_accessor*", "a")]),
                    FuncSig("long", "gribAccessorGetNativeType", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("long", "grib_get_next_position_offset", [Arg("grib_accessor*", "a")]),
                    FuncSig("long", "gribGetNextPositionOffset", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("long", "grib_string_length", [Arg("grib_accessor*", "a")]),
                    FuncSig("long", "gribStringLength", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("long", "grib_byte_offset", [Arg("grib_accessor*", "a")]),
                    FuncSig("long", "gribByteOffset", [Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("long", "grib_byte_count", [Arg("grib_accessor*", "a")]),
                    FuncSig("long", "gribByteCount", [Arg("AccessorPtr", "ptr")])),

    # =============================================================================

    FuncSigMapping( FuncSig("int", "grib_value_count", [Arg("grib_accessor*", "a"), Arg("long*", "count")]),
                    FuncSig("GribStatus", "gribValueCount", [Arg("AccessorPtr", "ptr"), Arg("long&", "count")])),

    FuncSigMapping( FuncSig("int", "grib_accessor_notify_change", [Arg("grib_accessor*", "a"), Arg("grib_accessor*", "changed")]),
                    FuncSig("GribStatus", "gribAccessorNotifyChange", [Arg("AccessorPtr", "ptr"), Arg("AccessorPtr", "changed")])),

    # =============================================================================

    FuncSigMapping( FuncSig("void", "grib_init_accessor", [Arg("grib_accessor*", "a"), Arg("const len", "len"), Arg("grib_arguments*", "args")]),
                    FuncSig("void", "gribInitAccessor", [Arg("AccessorPtr", "ptr"), Arg("const long", "len"), Arg("AccessorInitData", "args")])),

    FuncSigMapping( FuncSig("void", "grib_accessor_delete", [Arg("grib_context*", "ctx"), Arg("grib_accessor*", "a")]),
                    FuncSig("void", "gribAccessorDelete", [NONE_VALUE, Arg("AccessorPtr", "ptr")])),

    FuncSigMapping( FuncSig("void", "grib_update_size", [Arg("grib_accessor*", "a"), Arg("size_t", "len")]),
                    FuncSig("void", "gribUpdateSize", [Arg("AccessorPtr", "ptr"), Arg("size_t", "len")])),

    FuncSigMapping( FuncSig("void", "grib_resize", [Arg("grib_accessor*", "a"), Arg("size_t", "new_size")]),
                    FuncSig("void", "gribResize", [Arg("AccessorPtr", "ptr"), Arg("size_t", "newSize")])),

    FuncSigMapping( FuncSig("size_t", "grib_preferred_size", [Arg("grib_accessor*", "a"), Arg("int", "from_handle")]),
                    FuncSig("size_t", "gribPreferredSize", [Arg("AccessorPtr", "ptr"), Arg("int", "fromHandle")])),

    # =============================================================================

    FuncSigMapping( FuncSig("int", "grib_nearest_smaller_value", [Arg("grib_accessor*", "a"), Arg("double", "val"), Arg("double*", "nearest")]),
                    FuncSig("GribStatus", "gribNearestSmallerValue", [Arg("AccessorPtr", "ptr"), Arg("double", "val"), Arg("double&", "nearest")])),

    FuncSigMapping( FuncSig("int", "grib_compare_accessors", [Arg("grib_accessor*", "a1"), Arg("grib_accessor*", "a2"), Arg("int", "compare_flags")]),
                    FuncSig("GribStatus", "gribCompareAccessors", [Arg("AccessorPtr", "ptr1"), Arg("AccessorPtr", "ptr2"), Arg("int", "compareFlags")])),

    FuncSigMapping( FuncSig("int", "grib_accessor_add_attribute", [Arg("grib_accessor*", "a"), Arg("grib_accessor*", "attr"), Arg("int", "nest_if_clash")]),
                    FuncSig("GribStatus", "gribAccessorAddAttribute", [Arg("AccessorPtr", "ptr"), Arg("AccessorPtr", "attr"), Arg("int", "nextIfClash")])),

    FuncSigMapping( FuncSig("int", "grib_accessor_replace_attribute", [Arg("grib_accessor*", "a"), Arg("grib_accessor*", "attr")]),
                    FuncSig("GribStatus", "gribAccessorReplaceAttribute", [Arg("AccessorPtr", "ptr"), Arg("AccessorPtr", "attr")])),

    FuncSigMapping( FuncSig("int", "grib_accessor_delete_attribute", [Arg("grib_accessor*", "a"), Arg("const char*", "name")]),
                    FuncSig("GribStatus", "gribAccessorDeleteAttribute", [Arg("AccessorPtr", "ptr"), Arg("std::string", "name")])),

    FuncSigMapping( FuncSig("int", "grib_accessor_has_attributes", [Arg("grib_accessor*", "a")]),
                    FuncSig("GribStatus", "gribAccessorHasAttributes", [Arg("AccessorPtr", "ptr")])),

    # =============================================================================

    FuncSigMapping( FuncSig("grib_accessor*", "grib_accessor_clone", [Arg("grib_accessor*", "a"), Arg("grib_section*", "s"), Arg("int*", "err")]),
                    FuncSig("AccessorPtr", "gribAccessorClone", [Arg("AccessorPtr", "ptr"), NONE_VALUE, Arg("int&", "err")])),

    FuncSigMapping( FuncSig("grib_accessor*", "grib_accessor_get_attribute", [Arg("grib_accessor*", "a"), Arg("const char*", "name")]),
                    FuncSig("AccessorPtr", "gribAccessorGetAttribute", [Arg("AccessorPtr", "ptr"), Arg("const std::string", "name")])),

    FuncSigMapping( FuncSig("grib_accessors_list*", "grib_accessors_list_last", [Arg("grib_accessors_list*", "al")]),
                    FuncSig("std::vector<AccessorPtr>", "gribAccessorsListLast", [Arg("std::vector<AccessorPtr>", "ptrList")])),

]
