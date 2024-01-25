
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.funcsig import FuncSig
from code_object.arg import Arg
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes

# NOTE: The C FuncSig variable names may be different so should only be used as a guide (some are missing)
grib_accessor_virtual_member_funcsig_mapping = [
    # static int pack_TYPE( a, const TYPE* v, size_t* len)
    FuncSigMapping(FuncSig("int", "pack_string", [Arg("grib_accessor*", "a"), Arg("const char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::string const&", "stringValue"),  Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_long", [Arg("grib_accessor*", "a"), Arg("const long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::vector<long> const&", "longValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_double", [Arg("grib_accessor*", "a"), Arg("const double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::vector<double> const&", "doubleValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "pack_float", [Arg("grib_accessor*", "a"), Arg("const float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::vector<float> const&", "floatValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                        
    FuncSigMapping(FuncSig("int", "pack_string_array", [Arg("grib_accessor*", "a"), Arg("const char**", ""), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::vector<StringArray> const&", "stringValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                        
    FuncSigMapping(FuncSig("int", "pack_bytes", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", ""), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("std::vector<unsigned char> const&", "byteValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_TYPE(grib_accessor* a, TYPE* v, size_t* len)
    FuncSigMapping(FuncSig("int", "unpack_string", [Arg("grib_accessor*", "a"), Arg("char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::string&", "stringValue"),  Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_long", [Arg("grib_accessor*", "a"), Arg("long*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::vector<long>&", "longValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_double", [Arg("grib_accessor*", "a"), Arg("double*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::vector<double>&", "doubleValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_float", [Arg("grib_accessor*", "a"), Arg("float*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::vector<float>&", "floatValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_string_array", [Arg("grib_accessor*", "a"), Arg("char**", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::vector<StringArray>&", "stringValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    FuncSigMapping(FuncSig("int", "unpack_bytes", [Arg("grib_accessor*", "a"), Arg("unsigned char*", "v"), Arg("size_t*", "len")]),
                    FuncSig("GribStatus", "unpack", [Arg.NONE, Arg("std::vector<unsigned char>&", "byteValues"), Arg.NONE]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_TYPE_element(grib_accessor*, size_t i, TYPE* val);
    FuncSigMapping(FuncSig("int", "unpack_double_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("double*", "val")]),
                    FuncSig("GribStatus", "unpackElement", [Arg.NONE, Arg("std::size_t", "index"), Arg("double&", "val")])),
    FuncSigMapping(FuncSig("int", "unpack_float_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("float*", "val")]),
                    FuncSig("GribStatus", "unpackElement", [Arg.NONE, Arg("std::size_t", "index"), Arg("float&", "val")])),

    # static int unpack_TYPE_element_set(grib_accessor*, const size_t* index_array, size_t len, TYPE* val_array);
    FuncSigMapping(FuncSig("int", "unpack_double_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                    FuncSig("GribStatus", "unpackElementSet", [Arg.NONE, Arg("std::vector<std::size_t> const&", "indexArray"), Arg.NONE, Arg("std::vector<double>&", "valArray")]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
    FuncSigMapping(FuncSig("int", "unpack_float_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("float*", "val_array")]),
                    FuncSig("GribStatus", "unpackElementSet", [Arg.NONE, Arg("std::vector<std::size_t> const&", "indexArray"), Arg.NONE, Arg("std::vector<float>&", "valArray")]),
                    ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

    # static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
    FuncSigMapping(FuncSig("int", "unpack_double_subarray", [Arg("grib_accessor*", ""), Arg("double*", "val"), Arg("size_t", "start"), Arg("size_t", "len")]),
                    FuncSig("GribStatus", "unpackSubArray", [Arg.NONE, Arg("std::vector<double>&", "values"), Arg.NONE, Arg("std::size_t", "start")])),

    # Double arg functions of the form: RET func_name(grib_accessor*, TYPE);
    # static int pack_expression(grib_accessor*, grib_expression*);
    FuncSigMapping(FuncSig("int", "pack_expression", [Arg("grib_accessor*", ""), Arg("grib_expression*", "")]),
                    FuncSig("GribStatus", "pack", [Arg.NONE, Arg("grib_expression const&", "expression")])),
    # static int value_count(grib_accessor*, long*);
    FuncSigMapping(FuncSig("int", "value_count", [Arg("grib_accessor*", ""), Arg("long*", "")]),
                    FuncSig("GribStatus", "valueCount", [Arg.NONE, Arg("long&", "count")])),
    # static void update_size(grib_accessor*, size_t);
    FuncSigMapping(FuncSig("int", "update_size", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                    FuncSig("void", "updateSize", [Arg.NONE, Arg("std::size_t", "s")])),
    # static size_t preferred_size(grib_accessor*, int);
    FuncSigMapping(FuncSig("int", "preferred_size", [Arg("grib_accessor*", ""), Arg("int", "")]),
                    FuncSig("std::size_t", "preferredSize", [Arg.NONE, Arg("int", "fromHandle")])),
    # static void resize(grib_accessor*,size_t);
    FuncSigMapping(FuncSig("int", "resize", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                    FuncSig("void", "resize", [Arg.NONE, Arg("std::size_t", "newSize")])),

    # Single arg functions of the form: RET func_name(grib_accessor*);
    # static int get_native_type(grib_accessor*);
    FuncSigMapping(FuncSig("int", "get_native_type",  [Arg("grib_accessor*", "")]),
                    FuncSig("GribType", "nativeType", [Arg.NONE])),
    # static int pack_missing(grib_accessor*);
    FuncSigMapping(FuncSig("int", "pack_missing",  [Arg("grib_accessor*", "")]),
                    FuncSig("GribStatus", "packMissing", [Arg.NONE])),
    # static size_t string_length(grib_accessor*);
    FuncSigMapping(FuncSig("int", "string_length",  [Arg("grib_accessor*", "")]),
                    FuncSig("std::size_t", "stringLength", [Arg.NONE])),
    # static long byte_count(grib_accessor*);
    FuncSigMapping(FuncSig("int", "byte_count",  [Arg("grib_accessor*", "")]),
                    FuncSig("long", "byteCount", [Arg.NONE])),
    # static long byte_offset(grib_accessor*);
    FuncSigMapping(FuncSig("int", "byte_offset",  [Arg("grib_accessor*", "")]),
                    FuncSig("long", "byteOffset", [Arg.NONE])),

    # Other functions
    # static void dump(grib_accessor*, grib_dumper*);
    FuncSigMapping(FuncSig("int", "dump", [Arg("grib_accessor*", ""), Arg("grib_dumper*", "")]),
                    FuncSig("void", "dump", [Arg.NONE, Arg.NONE])),
    # static int compare(grib_accessor*, grib_accessor*);
    FuncSigMapping(FuncSig("int", "compare", [Arg("grib_accessor*", ""), Arg("grib_accessor*", "")]),
                    FuncSig("bool", "compare", [Arg.NONE, Arg("AccessorData const&", "rhs")])),
    # static grib_accessor* make_clone(grib_accessor*, grib_section*, int*);
    FuncSigMapping(FuncSig("int", "make_clone", [Arg("grib_accessor*", ""), Arg("grib_section*", ""), Arg("int*", "")]),
                    FuncSig("AccessorDataPtr", "clone", [Arg.NONE, Arg.NONE, Arg.NONE])),
    # static int is_missing(grib_accessor* a)
    FuncSigMapping(FuncSig("int", "is_missing", [Arg("grib_accessor*", "")]),
                    FuncSig("bool", "isMissing", [Arg.NONE])),

    # Excluded functions
    # static long next_offset(grib_accessor*);
    #FuncSigMapping(FuncSig("int", "next_offset",  [Arg("grib_accessor*", "")]),
    #                FuncSig(Arg.NONE, Arg.NONE, [Arg.NONE])),
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
