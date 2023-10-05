# Transforms that apply to AccessorData member functions

import re
from convert_debug import debug_line
from arg_transforms import Arg

# Represent a function signature
class FuncSig:
    def __init__(self, ret, name, args) -> None:
        self.ret = ret
        self.name = name
        self.args = args

# The following static dictionary defines well-known conversions from C to the equivalent
# AccessorData virtual functions
# [Key]     Name of an inherited function as defined in the C code (func_name in the functions below)
# [Value]   Signature for the equivalent function in C++
#           The argument list provides a 1:1 mapping from the C equivalent function - this map is also used 
#           when replacing the use of these in the function body
#           The C signature is shown in the comments to make it easier to identify which C arg maps to C++
#           Note: some C args don't have a C++ equivalent, so are listed as None to maintain correct mapping
accessor_member_func_conversions = {
    # static int pack_TYPE(grib_accessor* a, const TYPE* v, size_t* len)
    "pack_string"         : FuncSig("GribStatus", "pack",   [None, Arg("std::string const&",              "value"),  None]),
    "pack_long"           : FuncSig("GribStatus", "pack",   [None, Arg("std::vector<long> const&",        "values"), None]),
    "pack_double"         : FuncSig("GribStatus", "pack",   [None, Arg("std::vector<double> const&",      "values"), None]),
    "pack_float"          : FuncSig("GribStatus", "pack",   [None, Arg("std::vector<float> const&",       "values"), None]),
    "pack_string_array"   : FuncSig("GribStatus", "pack",   [None, Arg("std::vector<StringArray> const&", "values"), None]),
    "pack_bytes"          : FuncSig("GribStatus", "pack",   [None, Arg("std::vector<std::byte> const&",   "values"), None]),

    # static int unpack_TYPE(grib_accessor* a, TYPE* v, size_t* len)
    "unpack_string"       : FuncSig("GribStatus", "unpack", [None, Arg("std::string&",                    "value"),  None]),
    "unpack_long"         : FuncSig("GribStatus", "unpack", [None, Arg("std::vector<long>&",              "values"), None]),
    "unpack_double"       : FuncSig("GribStatus", "unpack", [None, Arg("std::vector<double>&",            "values"), None]),
    "unpack_float"        : FuncSig("GribStatus", "unpack", [None, Arg("std::vector<float>&",             "values"), None]),
    "unpack_string_array" : FuncSig("GribStatus", "unpack", [None, Arg("std::vector<StringArray>&",       "values"), None]),
    "unpack_bytes"        : FuncSig("GribStatus", "unpack", [None, Arg("std::vector<std::byte>&",         "values"), None]),

    # static int unpack_TYPE_element(grib_accessor*, size_t i, TYPE* val);
    "unpack_double_element"     : FuncSig("GribStatus", "unpackElement",    [None, Arg("std::size_t", "index"), Arg("double&", "val")]),
    "unpack_float_element"      : FuncSig("GribStatus", "unpackElement",    [None, Arg("std::size_t", "index"), Arg("float&", "val")]),
    
    # static int unpack_TYPE_element_set(grib_accessor*, const size_t* index_array, size_t len, TYPE* val_array);
    "unpack_double_element_set" : FuncSig("GribStatus", "unpackElementSet", [None, Arg("std::vector<std::size_t> const&", "indexArray"), None, Arg("std::vector<double>&", "valArray")]),
    "unpack_float_element_set"  : FuncSig("GribStatus", "unpackElementSet", [None, Arg("std::vector<std::size_t> const&", "indexArray"), None, Arg("std::vector<float>&", "valArray")]),
    
    # static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
    "unpack_double_subarray"    : FuncSig("GribStatus", "unpackSubArray",   [None, Arg("std::vector<double>&", "values"), None, Arg("std::size_t", "start")]),
    
    # Double arg functions of the form: RET func_name(grib_accessor*, TYPE);
    # static int pack_expression(grib_accessor*, grib_expression*);
    "pack_expression" : FuncSig("GribStatus", "pack",           [None, Arg("grib_expression const&", "expression")]),
    # static int value_count(grib_accessor*, long*);
    "value_count"     : FuncSig("long", "valueCount",           [None, None]),
    # static void update_size(grib_accessor*, size_t);
    "update_size"     : FuncSig("void", "updateSize",           [None, Arg("std::size_t", "s")]),
    # static size_t preferred_size(grib_accessor*, int);
    "preferred_size"  : FuncSig("std::size_t", "preferredSize", [None, Arg("int", "fromHandle")]),
    # static void resize(grib_accessor*,size_t);
    "resize"          : FuncSig("void", "resize",               [None, Arg("std::size_t", "newSize")]),

    # Single arg functions of the form: RET func_name(grib_accessor*);
    # static int get_native_type(grib_accessor*);
    "get_native_type" : FuncSig("GribType", "nativeType",       [None]),
    # static int pack_missing(grib_accessor*);
    "pack_missing"    : FuncSig("GribStatus", "packMissing",    [None]),
    # static size_t string_length(grib_accessor*);
    "string_length"   : FuncSig("std::size_t", "stringLength",  [None]),
    # static long byte_count(grib_accessor*);
    "byte_count"      : FuncSig("long", "byteCount",            [None]),
    # static long byte_offset(grib_accessor*);
    "byte_offset"     : FuncSig("long", "byteOffset",           [None]),
}

def return_type(func_name):
    if func_name in accessor_member_func_conversions:
        return accessor_member_func_conversions[func_name].ret
    else:
        return None

def transformed_name(func_name):
    if func_name in accessor_member_func_conversions:
        return accessor_member_func_conversions[func_name].name
    else:
        return None

def transformed_args(func_name):
    if func_name in accessor_member_func_conversions:
        return accessor_member_func_conversions[func_name].args
    else:
        return None
