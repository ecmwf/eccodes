
import default.cparser as cparser
import default.cfuncsig_converter as cfuncsig_converter
import default.cfuncsig_pointer_converter as cfuncsig_pointer_converter
import default.ccode as ccode
import default.ccode_converter as ccode_converter
import default.cppcode as cppcode
import default.cast_parser as cast_parser

# The manifest provides generic access to all the components required to run the parser for this class of code
#
# All functions must be implemented for the root convert.py script to locate the correct classes etc
#
# This is the default manifest, which is used when no other C code type is specified, 
# and therefore performs a default C to C++ conversion

class Manifest:
    def __init__(self):
        pass

    @property
    def CPARSER(self):
        return cparser.CParser
    
    @property
    def ignore_file_list(self):
        return []

    @property
    def CFUNCSIG_CONVERTER(self):
        return cfuncsig_converter.CFuncSigConverter
    
    @property
    def CFUNCSIG_POINTER_CONVERTER(self):
        return cfuncsig_pointer_converter.CFuncSigPointerConverter
    
    @property
    def CCODE(self):
        return ccode.CCode

    @property
    def CCODE_CONVERTER(self):
        return ccode_converter.CCodeConverter

    @property
    def CPP_CODE(self):
        return cppcode.CppCode

    @property
    def CAST_PARSER(self):
        return cast_parser.CASTParser

MANIFEST_CLASS=Manifest
