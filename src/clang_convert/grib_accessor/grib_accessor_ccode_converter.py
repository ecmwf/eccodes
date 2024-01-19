
import utils.debug as debug
import os
import default.default_ccode_converter as default_ccode_converter
import default.default_cppcode as default_cppcode
import utils.standard_transforms as standard_transforms
from grib_accessor.conversion_data.virtual_member_functions import grib_accessor_virtual_member_funcsig_mapping 
from grib_accessor.conversion_data.member_functions import grib_accessor_member_funcsig_mapping
import grib_accessor.conversion_data.member_functions as member_functions
import grib_accessor.conversion_data.virtual_member_functions as virtual_member_functions

prefix = "grib_accessor_class_"
rename = {
    "Gen": "Accessor",      # "Generic",
    "Md5": "Md5Sum",        # We rename because of macos case insensitive file system
    "Assert": "Assertion",  # Name clash with assert.h
}
# Converter for grib_accessor_ccode classes...
class GribAccessorCCodeConverter(default_ccode_converter.DefaultCCodeConverter):
    def __init__(self, ccode_instance) -> None:
        super().__init__(ccode_instance)

    # Convert e.g. grib_accessor_class_proj_string.cc to ProjStringData.cc
    def transform_file_name(self, name):
        name_stem, name_ext = os.path.splitext(os.path.basename(name))
        name_stem = self.transform_class_name(name_stem)
        return f"{name_stem}{name_ext}"

    # Convert e.g. grib_accessor_class_proj_string to ProjStringData
    def transform_class_name(self, name):
        name = name.replace(prefix, "")
        name = standard_transforms.transform_type_name(name)
        return rename.get(name, name) + "Data"

    # TODO update when the class is available...
    def create_cpp_code(self):
        cpp_filename = self.transform_file_name(self._ccode.cfilename)
        cpp_class_name = self.transform_class_name(self._ccode.class_name)
        cpp_super_class_name = self.transform_class_name(self._ccode.super_class_name)
        debug.line("create_cpp_code", f"cpp_filename=[{cpp_filename}] cpp_class_name=[{cpp_class_name}] cpp_super_class_name=[{cpp_super_class_name}]")
        self._cppcode = default_cppcode.DefaultCppCode(cpp_filename, cpp_class_name, cpp_super_class_name)

    def initialise_conversion_data(self):
        self._conversion_data.set_funcsig_mappings(grib_accessor_virtual_member_funcsig_mapping)
        for mapping in grib_accessor_member_funcsig_mapping:
            self._conversion_data.add_to_funcsig_mappings(mapping)
        # Add the member functions
        self._conversion_data.set_member_function_names(member_functions.member_function_names +
                                                        virtual_member_functions.virtual_member_function_names)

    def set_function_specific_conversion_data(self, function_name):
        pass
