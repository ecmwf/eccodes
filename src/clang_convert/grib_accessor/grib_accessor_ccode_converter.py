
import utils.debug as debug
import os
import cpp_code.code_info as code_info
import default.default_ccode_converter as default_ccode_converter
import utils.standard_transforms as standard_transforms
from grib_accessor.supporting.virtual_member_functions import grib_accessor_virtual_member_funcsig_mapping 
from grib_accessor.supporting.member_functions import grib_accessor_member_funcsig_mapping
import grib_accessor.supporting.member_functions as member_functions
import grib_accessor.supporting.virtual_member_functions as virtual_member_functions
import grib_accessor.supporting.includes as includes
import grib_accessor.supporting.type_mappings as type_mappings
from code_object.declaration_specifier import DeclSpec

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

    def create_code_info(self):
        cpp_filename = self.transform_file_name(self._ccode.cfilename)
        cpp_class_name = self.transform_class_name(self._ccode.class_name)
        cpp_super_class_name = self.transform_class_name(self._ccode.super_class_name)
        debug.line("create_code_info", f"cpp_filename=[{cpp_filename}] cpp_class_name=[{cpp_class_name}] cpp_super_class_name=[{cpp_super_class_name}]")

        info = code_info.CodeInfo(cpp_filename, cpp_class_name, cpp_super_class_name)
        info.add_namespace("eccodes")
        info.add_namespace("accessor")

        return info

    def initialise_conversion_data(self):
        for mapping in grib_accessor_member_funcsig_mapping:
            self._conversion_data.add_member_funcsig_mapping(mapping)

        for mapping in grib_accessor_virtual_member_funcsig_mapping:
            self._conversion_data.add_virtual_member_funcsig_mapping(mapping)

        type_mappings.add_type_mappings_to_conversion_data(self._conversion_data)

        # Add C class name pointer as "do not convert" (e.g. grib_accessor_class_proj_string* -> NoneDeclSpec)
        debug.line("initialise_conversion_data", f"Adding funcbody mapping for Accessor name=[{self._ccode.accessor_name}]")
        self._conversion_data.add_funcbody_type_mapping(DeclSpec.from_decl_specifier_seq(self._ccode.accessor_name+"*"), DeclSpec.NONE)

    def set_function_specific_conversion_data(self, function_name):
        pass

    def add_includes(self):
        # Header includes
        if self._conversion_data.info.super_class_name == "AccessorData":
            self._conversion_data.info.header_includes.append("/".join(["AccessorData", self._conversion_data.info.super_class_name + ".h"]))
        else:
            self._conversion_data.info.header_includes.append(f"{self._conversion_data.info.super_class_name}.h")

        for inc in includes.grib_accessor_header_includes:
            self._conversion_data.info.header_includes.append(inc)

        # Source includes
        self._conversion_data.info.source_includes.append(f"\"{self._conversion_data.info.class_name}.h\"")
        
        for inc in includes.grib_accessor_source_includes:
            self._conversion_data.info.source_includes.append(inc)


        # TODO: Class-specific includes

        return super().add_includes()
    
    def is_const_member_function(self, function_name):
        return function_name in virtual_member_functions.const_virtual_member_function_names
