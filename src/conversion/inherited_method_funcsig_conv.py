
from method_funcsig_conv import *
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

class InheritedMethodFuncSigConverter(MethodFuncSigConverter):
    # NOTE: The C FuncSig variable names may be different so should only be used as a guide (some are missing)
    inherited_method_conversions = [
        # static int pack_TYPE( a, const TYPE* v, size_t* len)
        FuncSigMapping(FuncSig("int", "pack_string", [Arg("grib_accessor*", "a"), Arg("const char*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::string const&", "value"),  None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "pack_long", [Arg("grib_accessor*", "a"), Arg("const long*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::vector<long> const&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "pack_double", [Arg("grib_accessor*", "a"), Arg("const double*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::vector<double> const&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "pack_float", [Arg("grib_accessor*", "a"), Arg("const float*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::vector<float> const&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                          
        FuncSigMapping(FuncSig("int", "pack_string_array", [Arg("grib_accessor*", "a"), Arg("const char**", ""), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::vector<StringArray> const&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
                          
        FuncSigMapping(FuncSig("int", "pack_bytes", [Arg("grib_accessor*", "a"), Arg("const unsigned char*", ""), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "pack", [None, Arg("std::vector<std::byte> const&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        # static int unpack_TYPE(grib_accessor* a, TYPE* v, size_t* len)
        FuncSigMapping(FuncSig("int", "unpack_string", [Arg("grib_accessor*", "a"), Arg("char*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::string&", "value"),  None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "unpack_long", [Arg("grib_accessor*", "a"), Arg("long*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::vector<long>&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "unpack_double", [Arg("grib_accessor*", "a"), Arg("double*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::vector<double>&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "unpack_float", [Arg("grib_accessor*", "a"), Arg("float*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::vector<float>&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "unpack_string_array", [Arg("grib_accessor*", "a"), Arg("char**", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::vector<StringArray>&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        FuncSigMapping(FuncSig("int", "unpack_bytes", [Arg("grib_accessor*", "a"), Arg("unsigned char*", "v"), Arg("size_t*", "len")]),
                       FuncSig("GribStatus", "unpack", [None, Arg("std::vector<std::byte>&", "values"), None]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),

        # static int unpack_TYPE_element(grib_accessor*, size_t i, TYPE* val);
        FuncSigMapping(FuncSig("int", "unpack_double_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("double*", "val")]),
                       FuncSig("GribStatus", "unpackElement", [None, Arg("std::size_t", "index"), Arg("double&", "val")])),
        FuncSigMapping(FuncSig("int", "unpack_float_element", [Arg("grib_accessor*", ""), Arg("size_t", "i"), Arg("float*", "val")]),
                       FuncSig("GribStatus", "unpackElement", [None, Arg("std::size_t", "index"), Arg("float&", "val")])),
    
        # static int unpack_TYPE_element_set(grib_accessor*, const size_t* index_array, size_t len, TYPE* val_array);
        FuncSigMapping(FuncSig("int", "unpack_double_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("double*", "val_array")]),
                       FuncSig("GribStatus", "unpackElementSet", [None, Arg("std::vector<std::size_t> const&", "indexArray"), None, Arg("std::vector<double>&", "valArray")])),
        FuncSigMapping(FuncSig("int", "unpack_float_element_set", [Arg("grib_accessor*", ""), Arg("const size_t*", "index_array"), Arg("size_t", "len"), Arg("float*", "val_array")]),
                       FuncSig("GribStatus", "unpackElementSet", [None, Arg("std::vector<std::size_t> const&", "indexArray"), None, Arg("std::vector<float>&", "valArray")])),
    
        # static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
        FuncSigMapping(FuncSig("int", "unpack_double_subarray", [Arg("grib_accessor*", ""), Arg("double*", "val"), Arg("size_t", "start"), Arg("size_t", "len")]),
                       FuncSig("GribStatus", "unpackSubArray", [None, Arg("std::vector<double>&", "values"), None, Arg("std::size_t", "start")])),
    
        # Double arg functions of the form: RET func_name(grib_accessor*, TYPE);
        # static int pack_expression(grib_accessor*, grib_expression*);
        FuncSigMapping(FuncSig("int", "pack_expression", [Arg("grib_accessor*", ""), Arg("grib_expression*", "")]),
                       FuncSig("GribStatus", "pack", [None, Arg("grib_expression const&", "expression")])),
        # static int value_count(grib_accessor*, long*);
        FuncSigMapping(FuncSig("int", "value_count", [Arg("grib_accessor*", ""), Arg("long*", "")]),
                       FuncSig("long", "valueCount", [None, None])),
        # static void update_size(grib_accessor*, size_t);
        FuncSigMapping(FuncSig("int", "update_size", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                       FuncSig("void", "updateSize", [None, Arg("std::size_t", "s")])),
        # static size_t preferred_size(grib_accessor*, int);
        FuncSigMapping(FuncSig("int", "preferred_size", [Arg("grib_accessor*", ""), Arg("int", "")]),
                       FuncSig("std::size_t", "preferredSize", [None, Arg("int", "fromHandle")])),
        # static void resize(grib_accessor*,size_t);
        FuncSigMapping(FuncSig("int", "resize", [Arg("grib_accessor*", ""), Arg("size_t", "")]),
                       FuncSig("void", "resize", [None, Arg("std::size_t", "newSize")])),

        # Single arg functions of the form: RET func_name(grib_accessor*);
        # static int get_native_type(grib_accessor*);
        FuncSigMapping(FuncSig("int", "get_native_type",  [Arg("grib_accessor*", "")]),
                       FuncSig("GribType", "nativeType", [None])),
        # static int pack_missing(grib_accessor*);
        FuncSigMapping(FuncSig("int", "pack_missing",  [Arg("grib_accessor*", "")]),
                       FuncSig("GribStatus", "packMissing", [None])),
        # static size_t string_length(grib_accessor*);
        FuncSigMapping(FuncSig("int", "string_length",  [Arg("grib_accessor*", "")]),
                       FuncSig("std::size_t", "stringLength", [None])),
        # static long byte_count(grib_accessor*);
        FuncSigMapping(FuncSig("int", "byte_count",  [Arg("grib_accessor*", "")]),
                       FuncSig("long", "byteCount", [None])),
        # static long byte_offset(grib_accessor*);
        FuncSigMapping(FuncSig("int", "byte_offset",  [Arg("grib_accessor*", "")]),
                       FuncSig("long", "byteOffset", [None])),

        # Other functions
        # static void dump(grib_accessor*, grib_dumper*);
        FuncSigMapping(FuncSig("int", "dump", [Arg("grib_accessor*", ""), Arg("grib_dumper*", "")]),
                       FuncSig("void", "dump", [None, None])),
        # static int compare(grib_accessor*, grib_accessor*);
        FuncSigMapping(FuncSig("int", "compare", [Arg("grib_accessor*", ""), Arg("grib_accessor*", "")]),
                       FuncSig("bool", "compare", [None, Arg("AccessorData const&", "rhs")])),
        # static grib_accessor* make_clone(grib_accessor*, grib_section*, int*);
        FuncSigMapping(FuncSig("int", "make_clone", [Arg("grib_accessor*", ""), Arg("grib_section*", ""), Arg("int*", "")]),
                       FuncSig("AccessorDataPtr", "clone", [None, None, None])),
        # static int is_missing(grib_accessor* a)
        FuncSigMapping(FuncSig("int", "is_missing", [Arg("grib_accessor*", "")]),
                       FuncSig("bool", "isMissing", [None])),

        # Excluded functions
        # static long next_offset(grib_accessor*);
        FuncSigMapping(FuncSig("int", "next_offset",  [Arg("grib_accessor*", "")]),
                       FuncSig(None, None, [None])),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.inherited_method_conversions)

# Helper to get the mapping of an inherited method from the c function name
# Use for transforms when the "virtual" method is referenced, but not implemented, in a Class
# Returns None if not mapping exists...
def cpp_inherited_method_mapping_for(cfunction_name):
    for mapping in InheritedMethodFuncSigConverter.inherited_method_conversions:
        if mapping.cfuncsig.name == cfunction_name:
            return mapping
        
    return None