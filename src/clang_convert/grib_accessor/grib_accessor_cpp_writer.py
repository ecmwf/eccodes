
import default.default_cpp_writer as default_cpp_writer
import os

# Write the supplied GRIB Accessor cppcode objects to disk
class GribAccessorCppWriter(default_cpp_writer.DefaultCppWriter):
    def __init__(self, output_path, cli_logger):
        super().__init__(output_path, cli_logger)
        
        self._j2_template_path = f"{os.path.dirname(__file__)}/j2"
        self._header_template = "grib_accessor_header.h.j2"
        self._source_template = "grib_accessor_source.cc.j2"
        self._cmakelists_template = "grib_accessor_cmakelists.txt.j2"

# Read from convert.py to get the correct class name
CPP_WRITER_CLASS=GribAccessorCppWriter