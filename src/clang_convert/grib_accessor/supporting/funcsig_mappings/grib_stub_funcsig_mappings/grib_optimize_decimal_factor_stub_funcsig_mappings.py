
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_optimize_decimal_factor_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_optimize_decimal_factor", [Arg("grib_accessor*", "a"), Arg("const char*", "reference_value"), Arg("const double", "pmax"),
                                                                    Arg("const double", "pmin"), Arg("const int", "knbit"), Arg("const int", "compat_gribex"),
                                                                    Arg("const int", "compat_32bit"), Arg("long*", "kdec"), Arg("long*", "kbin"), Arg("double*", "ref")]),
                    FuncSig("GribStatus", "gribOptimizeDecimalFactor", [NONE_VALUE, Arg("AccessorName", "referenceValue"), Arg("const double", "pmax"),
                                                                        Arg("const double", "pmin"), Arg("const int", "knbit"), Arg("const int", "compatGribex"),
                                                                        Arg("const int", "compat32bit"), Arg("long&", "kdec"), Arg("long&", "kbin"), Arg("double&", "ref")]))
    ]

