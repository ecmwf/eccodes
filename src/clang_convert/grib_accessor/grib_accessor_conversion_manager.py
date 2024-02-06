
import default.default_conversion_manager as default_conversion_manager
import grib_accessor.grib_accessor_ast_code_converter as grib_accessor_ast_code_converter
import grib_accessor.grib_accessor_ccode_converter as grib_accessor_ccode_converter
import grib_accessor.grib_accessor_cpp_writer as grib_accessor_cpp_writer

# Derived from the default class to support GribAccessor code

class GribAccessorConversionManager(default_conversion_manager.DefaultConversionManager):
    def __init__(self, output_path, cli_logger):
        super().__init__(output_path, cli_logger)

    # Conversion-specific : begin ========================================================
    @property
    def conversion_specific_path(self):
        return "/eccodes/accessor"

    @property
    def ast_code_converter_class(self):
        return grib_accessor_ast_code_converter.GribAccessorAstCodeConverter

    @property
    def ccode_converter_class(self):
        return grib_accessor_ccode_converter.GribAccessorCCodeConverter    

    @property
    def cpp_writer_class(self):
        return grib_accessor_cpp_writer.GribAccessorCppWriter

    @property
    def ignore_file_list(self):
        return [
            "grib_accessor_class_gen.cc",
        ]
    # Conversion-specific : end ==========================================================

    def get_parent_filename(self, ast_code_instance):
        for node in ast_code_instance.global_function_nodes:
            if node.type.spelling == "grib_accessor_class *":
                return node.spelling + ".cc"
        return None

# Read by convert.py
CONVERSION_MANAGER_CLASS=GribAccessorConversionManager