
import utils.debug as debug
import os
import default.default_ccode_converter as default_ccode_converter
import default.default_cppcode as default_cppcode
import code_object.cppfunction as cppfunction
import utils.standard_transforms as standard_transforms

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

