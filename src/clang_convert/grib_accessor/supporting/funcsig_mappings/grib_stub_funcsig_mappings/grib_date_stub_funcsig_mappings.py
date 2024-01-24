
from code_object.funcsig import FuncSig
from code_object_converter.supporting.arg_indexes import ArgIndexes
from code_object.arg import Arg
from code_object_converter.supporting.funcsig_mapping import FuncSigMapping

grib_date_stub_funcsig_mappings = [
    FuncSigMapping( FuncSig("long", "grib_date_to_julian", [Arg("long", "ddate")]),
                    FuncSig("long", "gribDateToJulian", [Arg("long", "ddate")])),

    FuncSigMapping( FuncSig("int", "grib_datetime_to_julian", [Arg("long", "year"), Arg("long", "month"), Arg("long", "day"), Arg("long", "hour"), Arg("long", "minute"), Arg("long", "second"), Arg("double*", "jd")]),
                    FuncSig("int", "gribDatetimeToJulian", [Arg("long", "year"), Arg("long", "month"), Arg("long", "day"), Arg("long", "hour"), Arg("long", "minute"), Arg("long", "second"), Arg("double&", "jd")])),

    FuncSigMapping( FuncSig("int", "grib_datetime_to_julian_d", [Arg("long", "year"), Arg("long", "month"), Arg("long", "day"), Arg("long", "hour"), Arg("long", "minute"), Arg("long", "second"), Arg("double*", "jd")]),
                    FuncSig("int", "gribDatetimeToJulianD", [Arg("long", "year"), Arg("long", "month"), Arg("long", "day"), Arg("long", "hour"), Arg("long", "minute"), Arg("long", "second"), Arg("double&", "jd")])),

    FuncSigMapping( FuncSig("long", "grib_julian_to_date", [Arg("long", "jdate")]),
                    FuncSig("long", "gribJulianToDate", [Arg("long", "jdate")])),

    FuncSigMapping( FuncSig("int", "grib_julian_to_datetime", [Arg("double", "jd"), Arg("long*", "year"), Arg("long*", "month"), Arg("long*", "day"), Arg("long*", "hour"), Arg("long*", "minute"), Arg("long*", "second")]),
                    FuncSig("int", "gribJulianToDatetime", [Arg("double", "jd"), Arg("long&", "year"), Arg("long&", "month"), Arg("long&", "day"), Arg("long&", "hour"), Arg("long&", "minute"), Arg("long&", "second")])),

]
