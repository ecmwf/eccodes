
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_optimize_decimal_factor_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_optimize_decimal_factor", [Arg("grib_accessor*", "a"), Arg("const char*", "reference_value"), Arg("const double", "pmax"),
                                                                    Arg("const double", "pmin"), Arg("const int", "knbit"), Arg("const int", "compat_gribex"),
                                                                    Arg("const int", "compat_32bit"), Arg("long*", "kdec"), Arg("long*", "kbin"), Arg("double*", "ref")]),
                    FuncSig("GribStatus", "gribOptimizeDecimalFactor", [None, Arg("AccessorName", "referenceValue"), Arg("const double", "pmax"),
                                                                        Arg("const double", "pmin"), Arg("const int", "knbit"), Arg("const int", "compatGribex"),
                                                                        Arg("const int", "compat32bit"), Arg("long&", "kdec"), Arg("long&", "kbin"), Arg("double&", "ref")]))
    ]

