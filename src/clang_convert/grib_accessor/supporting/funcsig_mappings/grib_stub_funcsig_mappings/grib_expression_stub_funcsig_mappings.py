
from code_object.funcsig import FuncSig
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

# Conversions for common Grib functions

grib_expression_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_expression_native_type", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g")]),
                    FuncSig("GribStatus", "gribExpressionNativeType", [NONE_VALUE, Arg("GribExpressionPtr", "g")])),

    FuncSigMapping( FuncSig("int", "grib_expression_evaluate_long", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("long*", "result")]),
                    FuncSig("GribStatus", "gribExpressionEvaluateLong", [NONE_VALUE, Arg("GribExpressionPtr", "g"), Arg("long&", "result")])),

    FuncSigMapping( FuncSig("int", "grib_expression_evaluate_double", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("double*", "result")]),
                    FuncSig("GribStatus", "gribExpressionEvaluateDouble", [NONE_VALUE, Arg("GribExpressionPtr", "g"), Arg("double&", "result")])),

    FuncSigMapping( FuncSig("const char*", "grib_expression_evaluate_string", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("char*", "buf"), Arg("size_t*", "size"), Arg("int*", "err")]),
                    FuncSig("std::string", "gribExpressionEvaluateString", [NONE_VALUE, Arg("GribExpressionPtr", "g"), Arg("std::string", "buf"), NONE_VALUE, Arg("int&", "err")])),

    FuncSigMapping( FuncSig("const char*", "grib_expression_get_name", [Arg("grib_expression*", "g")]),
                    FuncSig("std::string", "gribExpressionGetName", [Arg("GribExpressionPtr", "g")])),

    FuncSigMapping( FuncSig("void", "grib_expression_print", [Arg("grib_context*", "ctx"), Arg("grib_expression*", "g"), Arg("grib_handle*", "f")]),
                    FuncSig("void", "gribExpressionPrint", [NONE_VALUE, Arg("GribExpressionPtr", "g"), NONE_VALUE])),

    FuncSigMapping( FuncSig("void", "grib_expression_free", [Arg("grib_context*", "ctx"), Arg("grib_expression*", "g")]),
                    FuncSig("void", "DELETE_THIS_grib_expression_free", [NONE_VALUE, NONE_VALUE])),

    FuncSigMapping( FuncSig("void", "grib_expression_add_dependency", [Arg("grib_expression*", "g"), Arg("grib_accessor*", "observer")]),
                    FuncSig("GribStatus", "gribExpressionAddDependency", [Arg("GribExpressionPtr", "g"), Arg("AccessorPtr", "observer")])),

    # TODO
    FuncSigMapping( FuncSig("int", "grib_expression_set_value", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("grib_values*", "v")]),
                    FuncSig("GribStatus", "gribExpressionSetValue", [NONE_VALUE, Arg("GribExpressionPtr", "g"), Arg("GribValuesPtr", "v")])),

]
