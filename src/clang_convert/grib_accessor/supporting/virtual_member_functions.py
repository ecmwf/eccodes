
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.funcsig import FuncSig
from code_object.arg import Arg
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.code_interface import NONE_VALUE

# NOTE: The C FuncSig variable names may be different so should only be used as a guide (some are missing)
grib_accessor_virtual_member_funcsig_mapping = [
    # static int pack_TYPE( a, const TYPE* v, size_t* len)
    FuncSigMapping(FuncSig("int", "pack_string", [Arg("grib_accessor*", "a"), Arg("const char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::string const&", "stringValue"),  NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_long", [Arg("grib_accessor*", "a"), Arg("const long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::vector<long> const&", "longValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_double", [Arg("grib_accessor*", "a"), Arg("const double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::vector<double> const&", "doubleValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_float", [Arg("grib_accessor*", "a"), Arg("const float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::vector<float> const&", "floatValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                        
    FuncSigMapping(FuncSig("int", "pack_string_array", [Arg("grib_accessor*", "a"), Arg("const char**", ""), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::vector<StringArray> const&", "stringValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                        
    FuncSigMapping(FuncSig("int", "pack_bytes", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", ""), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("std::vector<unsigned char> const&", "byteValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_TYPE(grib_accessor* a, TYPE* v, size_t* len)
    FuncSigMapping(FuncSig("int", "unpack_string", [Arg("grib_accessor*", "a"), Arg("char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::string&", "stringValue"),  NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_long", [Arg("grib_accessor*", "a"), Arg("long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::vector<long>&", "longValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_double", [Arg("grib_accessor*", "a"), Arg("double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::vector<double>&", "doubleValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_float", [Arg("grib_accessor*", "a"), Arg("float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::vector<float>&", "floatValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_string_array", [Arg("grib_accessor*", "a"), Arg("char**", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::vector<StringArray>&", "stringValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_bytes", [Arg("grib_accessor*", "a"), Arg("unsigned char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [NONE_VALUE, Arg("std::vector<unsigned char>&", "byteValues"), NONE_VALUE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_TYPE_element(grib_accessor*, size_t i, TYPE* val);
    FuncSigMapping(FuncSig("int", "unpack_double_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "unpackElement", [NONE_VALUE, Arg("std::size_t", "index"), Arg("double&", "val")])),
    FuncSigMapping(FuncSig("int", "unpack_float_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("float*", "val")]),
                    FuncSig("GribStatus", "unpackElement", [NONE_VALUE, Arg("std::size_t", "index"), Arg("float&", "val")])),

    # static int unpack_TYPE_element_set(grib_accessor*, const size_t* index_array, size_t len, TYPE* val_array);
    FuncSigMapping(FuncSig("int", "unpack_double_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "unpackElementSet", [NONE_VALUE, Arg("std::vector<std::size_t> const&", "indexArray"), NONE_VALUE, Arg("std::vector<double>&", "valArray")]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    FuncSigMapping(FuncSig("int", "unpack_float_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("float*", "val_array")]),
                    FuncSig("GribStatus", "unpackElementSet", [NONE_VALUE, Arg("std::vector<std::size_t> const&", "indexArray"), NONE_VALUE, Arg("std::vector<float>&", "valArray")]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
    FuncSigMapping(FuncSig("int", "unpack_double_subarray", [Arg("grib_accessor*", ""), Arg("double*", "val"), Arg("size_t", "start"), Arg("size_t", "len")]),
                    FuncSig("GribStatus", "unpackSubArray", [NONE_VALUE, Arg("std::vector<double>&", "values"), NONE_VALUE, Arg("std::size_t", "start")])),

    # Double arg functions of the form: RET func_name(grib_accessor*, TYPE);
    # static int pack_expression(grib_accessor*, grib_expression*);
    FuncSigMapping(FuncSig("int", "pack_expression", [Arg("grib_accessor*", ""), Arg("grib_expression*", "")]),
                    FuncSig("GribStatus", "pack", [NONE_VALUE, Arg("grib_expression const&", "expression")])),
    # static int value_count(grib_accessor*, long*);
    FuncSigMapping(FuncSig("int", "value_count", [Arg("grib_accessor*", ""), Arg("long*", "")]),
                    FuncSig("GribStatus", "valueCount", [NONE_VALUE, Arg("long&", "count")])),
    # static void update_size(grib_accessor*, size_t);
    FuncSigMapping(FuncSig("int", "update_size", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                    FuncSig("void", "updateSize", [NONE_VALUE, Arg("std::size_t", "s")])),
    # static size_t preferred_size(grib_accessor*, int);
    FuncSigMapping(FuncSig("int", "preferred_size", [Arg("grib_accessor*", ""), Arg("int", "")]),
                    FuncSig("std::size_t", "preferredSize", [NONE_VALUE, Arg("int", "fromHandle")])),
    # static void resize(grib_accessor*,size_t);
    FuncSigMapping(FuncSig("int", "resize", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                    FuncSig("void", "resize", [NONE_VALUE, Arg("std::size_t", "newSize")])),

    # Single arg functions of the form: RET func_name(grib_accessor*);
    # static int get_native_type(grib_accessor*);
    FuncSigMapping(FuncSig("int", "get_native_type",  [Arg("grib_accessor*", "")]),
                    FuncSig("GribType", "nativeType", [NONE_VALUE])),
    # static int pack_missing(grib_accessor*);
    FuncSigMapping(FuncSig("int", "pack_missing",  [Arg("grib_accessor*", "")]),
                    FuncSig("GribStatus", "packMissing", [NONE_VALUE])),
    # static size_t string_length(grib_accessor*);
    FuncSigMapping(FuncSig("int", "string_length",  [Arg("grib_accessor*", "")]),
                    FuncSig("std::size_t", "stringLength", [NONE_VALUE])),
    # static long byte_count(grib_accessor*);
    FuncSigMapping(FuncSig("int", "byte_count",  [Arg("grib_accessor*", "")]),
                    FuncSig("long", "byteCount", [NONE_VALUE])),
    # static long byte_offset(grib_accessor*);
    FuncSigMapping(FuncSig("int", "byte_offset",  [Arg("grib_accessor*", "")]),
                    FuncSig("long", "byteOffset", [NONE_VALUE])),

    # Other functions
    # static void dump(grib_accessor*, grib_dumper*);
    FuncSigMapping(FuncSig("int", "dump", [Arg("grib_accessor*", ""), Arg("grib_dumper*", "")]),
                    FuncSig("void", "dump", [NONE_VALUE, NONE_VALUE])),
    # static int compare(grib_accessor*, grib_accessor*);
    FuncSigMapping(FuncSig("int", "compare", [Arg("grib_accessor*", ""), Arg("grib_accessor*", "")]),
                    FuncSig("bool", "compare", [NONE_VALUE, Arg("AccessorData const&", "rhs")])),
    # static grib_accessor* make_clone(grib_accessor*, grib_section*, int*);
    FuncSigMapping(FuncSig("int", "make_clone", [Arg("grib_accessor*", ""), Arg("grib_section*", ""), Arg("int*", "")]),
                    FuncSig("AccessorDataPtr", "clone", [NONE_VALUE, NONE_VALUE, NONE_VALUE])),
    # static int is_missing(grib_accessor* a)
    FuncSigMapping(FuncSig("int", "is_missing", [Arg("grib_accessor*", "")]),
                    FuncSig("bool", "isMissing", [NONE_VALUE])),

    # Excluded functions
    # static long next_offset(grib_accessor*);
    FuncSigMapping(FuncSig("int", "next_offset",  [Arg("grib_accessor*", "")]),
                    NONE_VALUE),
]

virtual_member_function_names = [mapping.cfuncsig.name for mapping in grib_accessor_virtual_member_funcsig_mapping]

const_virtual_member_function_names = [
    "unpack_string",
    "unpack_long",
    "unpack_double",
    "unpack_float",
    "unpack_string_array",
    "unpack_bytes",
    "unpack_double_element",
    "unpack_float_element",
    "unpack_double_element_set",
    "unpack_float_element_set",
    "unpack_double_subarray",
    "value_count",
    "preferred_size",
    "resize",
    "get_native_type",
    "string_length",
    "byte_count",
    "byte_offset",
    "dump",
    "compare",
    "make_clone",
    "is_missing",
    "next_offset",
]
