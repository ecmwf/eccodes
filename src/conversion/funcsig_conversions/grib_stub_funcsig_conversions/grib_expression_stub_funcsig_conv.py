
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

# Conversions for common Grib functions

grib_expression_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_expression_native_type", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g")]),
                    FuncSig("GribStatus", "gribExpressionNativeType", [None, Arg("GribExpressionPtr", "g")])),

    FuncSigMapping( FuncSig("int", "grib_expression_evaluate_long", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("long*", "result")]),
                    FuncSig("GribStatus", "gribExpressionEvaluateLong", [None, Arg("GribExpressionPtr", "g"), Arg("long&", "result")])),

    FuncSigMapping( FuncSig("int", "grib_expression_evaluate_double", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("double*", "result")]),
                    FuncSig("GribStatus", "gribExpressionEvaluateDouble", [None, Arg("GribExpressionPtr", "g"), Arg("double&", "result")])),

    FuncSigMapping( FuncSig("const char*", "grib_expression_evaluate_string", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("char*", "buf"), Arg("size_t*", "size"), Arg("int*", "err")]),
                    FuncSig("std::string", "gribExpressionEvaluateString", [None, Arg("GribExpressionPtr", "g"), Arg("std::string", "buf"), None, Arg("int&", "err")])),

    FuncSigMapping( FuncSig("const char*", "grib_expression_get_name", [Arg("grib_expression*", "g")]),
                    FuncSig("std::string", "gribExpressionGetName", [Arg("GribExpressionPtr", "g")])),

    FuncSigMapping( FuncSig("void", "grib_expression_print", [Arg("grib_context*", "ctx"), Arg("grib_expression*", "g"), Arg("grib_handle*", "f")]),
                    FuncSig("void", "gribExpressionPrint", [None, Arg("GribExpressionPtr", "g"), None])),

    FuncSigMapping( FuncSig("void", "grib_expression_free", [Arg("grib_context*", "ctx"), Arg("grib_expression*", "g")]),
                    FuncSig(None, None, [None, None])),

    FuncSigMapping( FuncSig("void", "grib_expression_add_dependency", [Arg("grib_expression*", "g"), Arg("grib_accessor*", "observer")]),
                    FuncSig("GribStatus", "gribExpressionAddDependency", [Arg("GribExpressionPtr", "g"), Arg("AccessorPtr", "observer")])),

    # TODO
    FuncSigMapping( FuncSig("int", "grib_expression_set_value", [Arg("grib_handle*", "h"), Arg("grib_expression*", "g"), Arg("grib_values*", "v")]),
                    FuncSig("GribStatus", "gribExpressionSetValue", [None, Arg("GribExpressionPtr", "g"), Arg("GribValuesPtr", "v")])),

]
