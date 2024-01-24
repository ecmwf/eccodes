
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping
from code_object.code_interface import NONE_VALUE

grib_util_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("int", "grib_check_data_values_range", [Arg("grib_handle*", "h"), Arg("const double", "min_val"), Arg("const double", "max_val")]),
                    FuncSig("GribStatus", "gribCheckDataValuesRange", [NONE_VALUE, Arg("const double", "minVal"), Arg("const double", "maxVal")])),

    FuncSigMapping( FuncSig("int", "grib_is_earth_oblate", [Arg("grib_handle*", "h")]),
                    FuncSig("bool", "gribIsEarthOblate", [NONE_VALUE])),

    FuncSigMapping( FuncSig("int", "grib_producing_large_constant_fields", [Arg("int", "edition")]),
                    FuncSig("bool", "gribProducingLargeConstantFields", [Arg("int", "edition")])),

    FuncSigMapping( FuncSig("int", "grib_util_grib_data_quality_check", [Arg("grib_handle*", "h"), Arg("double", "min_val"), Arg("double", "max_val")]),
                    FuncSig("GribStatus", "gribUtilGribDataQualityCheck", [NONE_VALUE, Arg("double", "minVal"), Arg("double", "maxVal")])),
]
