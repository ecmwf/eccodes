
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object.funcsig import FuncSig
from code_object.arg import Arg
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes

grib_accessor_header_includes = [

]

grib_accessor_source_includes = [
    "\"Accessor.h\"", 
    "\"AccessorFactory.h\"", 
    "\"AccessorUtils/ConversionHelper.h\"",
    "\"AccessorUtils/ContainerHelper.h\"",
    "\"AccessorUtils/AccessorException.h\"",
    "\"GribCpp/GribCppIncludes.h\"",
    "\"GribStub/GribStubIncludes.h\"",
]
