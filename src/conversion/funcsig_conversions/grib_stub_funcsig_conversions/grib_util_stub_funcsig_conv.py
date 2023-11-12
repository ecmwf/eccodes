
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
from funcsig_mapping import FuncSigMapping

grib_util_stub_funcsig_conversions = [
    FuncSigMapping( FuncSig("int", "grib_check_data_values_range", [Arg("grib_handle*", "h"), Arg("const double", "min_val"), Arg("const double", "max_val")]),
                    FuncSig("GribStatus", "gribCheckDataValuesRange", [None, Arg("const double", "minVal"), Arg("const double", "maxVal")])),

    FuncSigMapping( FuncSig("int", "grib_is_earth_oblate", [Arg("grib_handle*", "h")]),
                    FuncSig("bool", "gribIsEarthOblate", [None])),

    FuncSigMapping( FuncSig("int", "grib_producing_large_constant_fields", [Arg("int", "edition")]),
                    FuncSig("bool", "gribProducingLargeConstantFields", [Arg("int", "edition")])),

    FuncSigMapping( FuncSig("int", "grib_util_grib_data_quality_check", [Arg("grib_handle*", "h"), Arg("double", "min_val"), Arg("double", "max_val")]),
                    FuncSig("GribStatus", "gribUtilGribDataQualityCheck", [None, Arg("double", "minVal"), Arg("double", "maxVal")])),
]
