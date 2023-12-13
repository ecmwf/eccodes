
import default.default_manifest as default_manifest
import grib_accessor.grib_accessor_cparser as grib_accessor_cparser
import grib_accessor.grib_accessor_ccode as grib_accessor_ccode
import grib_accessor.grib_accessor_ccode_converter as grib_accessor_ccode_converter

# The manifest provides generic access to all the components required to run the parser for this class of code
#
# All functions must be implemented for the root convert.py script to locate the correct classes etc

class GribAccessorManifest(default_manifest.Manifest):
    def __init__(self):
        super().__init__()

    @property
    def CPARSER(self):
        return grib_accessor_cparser.GribAccessorCParser
    
    @property
    def ignore_file_list(self):
        return [
            "grib_accessor_class_gen.cc",
        ]

    @property
    def CCODE(self):
        return grib_accessor_ccode.GribAccessorCCode

    @property
    def CCODE_CONVERTER(self):
        return grib_accessor_ccode_converter.GribAccessorCCodeConverter

    '''@property
    def CFUNCSIG_CONVERTER(self):
        return funcsig_converter.CFuncSigConverter'''

MANIFEST_CLASS=GribAccessorManifest
