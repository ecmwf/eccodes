
import default.default_converter as default_converter
import grib_accessor.grib_accessor_cfile_parser as grib_accessor_cfile_parser
import grib_accessor.grib_accessor_ccode_converter as grib_accessor_ccode_converter

# Derived from the default class to support GribAccessor code

class GribAccessorConverter(default_converter.DefaultConverter):
    def __init__(self, cli_logger):
        super().__init__(cli_logger)

    @property
    def cfile_parser_class(self):
        return grib_accessor_cfile_parser.GribAccessorCParser

    @property
    def ccode_converter_class(self):
        return grib_accessor_ccode_converter.GribAccessorCCodeConverter

    @property
    def ignore_file_list(self):
        return [
            "grib_accessor_class_gen.cc",
        ]

# Read from convert.py to get the correct class name
CONVERTER_CLASS=GribAccessorConverter
