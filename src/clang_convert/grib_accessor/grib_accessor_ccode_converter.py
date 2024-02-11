
import utils.debug as debug
import importlib
import os
import cpp_code.code_info as code_info
import default.default_ccode_converter as default_ccode_converter
import utils.standard_transforms as standard_transforms
from grib_accessor.supporting.virtual_member_functions import grib_accessor_virtual_member_funcsig_mapping 
from grib_accessor.supporting.member_functions import grib_accessor_member_funcsig_mapping
import grib_accessor.supporting.virtual_member_functions as virtual_member_functions
import grib_accessor.supporting.includes as includes
import grib_accessor.supporting.type_mappings as type_mappings
from code_object.declaration_specifier import DeclSpec
from code_object.code_interface import NONE_VALUE
import grib_accessor.supporting.funcsig_mappings.all_funcsig_mappings as all_funcsig_mappings
from grib_accessor.grib_accessor_conversion_pack.grib_accessor_conversion_validation import GribAccessorConversionValidation
import grib_accessor.supporting.grib_literal_mapping as grib_literal_mapping
import grib_accessor.supporting.grib_literal_mapping as grib_literal_mapping
import grib_accessor.supporting.arg_mappings as arg_mappings
import grib_accessor.supporting.data_member_mappings as data_member_mappings
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_type_info as grib_accessor_type_info
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_container_utils as grib_accessor_container_utils
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
import code_object_converter.conversion_pack.funcsig_mapping as funcsig_mapping

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
        self.load_conversion_pack_updates_class()

    # See if we have a function-specific updater, otherwise use the main-one
    def load_conversion_pack_updates_class(self):
        conversion_pack_updates_path="grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates"
        cclass_short_name = self._ccode.class_name.replace(prefix, "")
        accessor_conversion_pack_updates_mod_name = f"{cclass_short_name}_conversion_pack_updates"
        accessor_conversion_pack_updates_lib_name = f"{conversion_pack_updates_path}.{accessor_conversion_pack_updates_mod_name}"

        try:
            accessor_conversion_pack_updates_lib = importlib.import_module(accessor_conversion_pack_updates_lib_name)
            debug.line("function_specific_conversion_pack_updates", f"Loaded accessor_conversion_pack_updates_lib_name=[{accessor_conversion_pack_updates_lib_name}]")
            accessor_conversion_pack_updates_class_name = standard_transforms.transform_type_name(accessor_conversion_pack_updates_mod_name)
            accessor_conversion_pack_updates_class = getattr(accessor_conversion_pack_updates_lib, accessor_conversion_pack_updates_class_name)
            debug.line("function_specific_conversion_pack_updates", f"Loaded accessor_conversion_pack_updates_class_name=[{accessor_conversion_pack_updates_class_name}]")
            self._conversion_pack_updates = accessor_conversion_pack_updates_class()

        except ModuleNotFoundError:
            debug.line("function_specific_conversion_pack_updates", f"Could not find accessor_conversion_pack_updates_lib_name=[{accessor_conversion_pack_updates_lib_name}], using base version")
            self._conversion_pack_updates = base_conversion_pack_updates.BaseConversionPackUpdates()

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

    def function_specific_conversion_pack_updates(self, cfunction_name, conv_pack):
        self._conversion_pack_updates.apply_updates_for_cfunction(cfunction_name, conv_pack)
        super().function_specific_conversion_pack_updates(cfunction_name, conv_pack)

    # See if we have an Accessor-specific validator (e.g. ProjStringValidation),
    # Otherwise use the default
    def create_conversion_validation(self):
        validators_path="grib_accessor.grib_accessor_conversion_pack.validators"
        cclass_short_name = self._ccode.class_name.replace(prefix, "")
        accessor_validator_mod_name = f"{cclass_short_name}_validation"
        accessor_validator_lib_name = f"{validators_path}.{accessor_validator_mod_name}"

        try:
            accessor_validator_lib = importlib.import_module(accessor_validator_lib_name)
            debug.line("create_conversion_validation", f"Loaded accessor_validator lib name=[{accessor_validator_lib_name}]")
            accessor_validator_class_name = standard_transforms.transform_type_name(accessor_validator_mod_name)
            accessor_validation_class = getattr(accessor_validator_lib, accessor_validator_class_name)
            debug.line("create_conversion_validation", f"Loaded accessor_validator class name=[{accessor_validator_class_name}]")
            return accessor_validation_class()
        except ModuleNotFoundError:
            debug.line("create_conversion_validation", f"Could not find accessor_validator lib name=[{accessor_validator_lib_name}] - using default")

        return GribAccessorConversionValidation()

    def create_container_utils(self):
        return grib_accessor_container_utils.GribAccessorContainerUtils()

    def set_custom_conversion_data(self, conv_data):
        for mapping in grib_accessor_member_funcsig_mapping:
            conv_data.add_global_member_funcsig_mapping(mapping)

        # Store any member functions we have "discovered"
        # We don't yet have the C++ conversion so we'll add the C funcsig for C++ (it's a hack, but should be ok unless we later look
        # for the C++ name!)
        for cfunc in self._ccode.member_functions:
            if not conv_data.is_member_function(cfunc.funcsig.name):
                mapping = funcsig_mapping.FuncSigMapping(cfunc.funcsig, None)
                debug.line("set_custom_conversion_data", f"Adding <discovered> member function mapping (only cfuncsig available): [{debug.as_debug_string(cfunc.funcsig)}]->[None]")
                conv_data.add_global_member_funcsig_mapping(mapping)

        for mapping in grib_accessor_virtual_member_funcsig_mapping:
            conv_data.add_global_virtual_member_funcsig_mapping(mapping)

        self._conversion_pack_updates.add_funcsig_mappings_to_conversion_data(conv_data)

        all_funcsig_mappings.add_all_funcsig_mappings_to_conversion_data(conv_data)

        arg_mappings.add_arg_mappings_to_conversion_data(conv_data)

        type_mappings.add_type_mappings_to_conversion_data(conv_data)

        grib_literal_mapping.add_grib_literal_mappings_to_conversion_data(conv_data)

        data_member_mappings.add_data_member_mappings_to_conversion_data(conv_data)

        # Add C class name pointer as "do not convert" (e.g. grib_accessor_class_proj_string* -> NoneDeclSpec)
        debug.line("set_custom_conversion_data", f"Adding funcbody mapping for Accessor name=[{self._ccode.accessor_name}]")
        conv_data.add_funcbody_type_mapping(DeclSpec.from_decl_specifier_seq(self._ccode.accessor_name+"*"), NONE_VALUE)

    # Override to extend the type-info...
    @property
    def type_info(self):
         return grib_accessor_type_info.GribAccessorTypeInfo()

    def add_includes(self, conv_pack):
        # Header includes
        if conv_pack.conversion_data.info.super_class_name == "AccessorData":
            conv_pack.conversion_data.info.header_includes.append("/".join(["AccessorData", conv_pack.conversion_data.info.super_class_name + ".h"]))
        else:
            conv_pack.conversion_data.info.header_includes.append(f"{conv_pack.conversion_data.info.super_class_name}.h")

        for inc in includes.grib_accessor_header_includes:
            conv_pack.conversion_data.info.header_includes.append(inc)

        # Source includes
        conv_pack.conversion_data.info.source_includes.append(f"\"{conv_pack.conversion_data.info.class_name}.h\"")
        
        for inc in includes.grib_accessor_source_includes:
            conv_pack.conversion_data.info.source_includes.append(inc)


        # TODO: Class-specific includes

        return super().add_includes(conv_pack)
    
    def is_const_member_function(self, function_name, conv_pack):

        if function_name in ["init", "destroy"]:
            return False

        if conv_pack.conversion_data.is_virtual_member_function(function_name):
           return function_name in virtual_member_functions.const_virtual_member_function_names
        
        if function_name in self._code_info.non_const_member_function_names:
            return False

        if conv_pack.conversion_data.is_member_function(function_name):
            return True
        
        return False

    def post_process_function_calls(self):
        debug.line("post_process_function_calls", f"Function calls summary:")

        for funcsig_call in self._code_info.function_calls:
            debug.line("post_process_function_calls", f" -> [{debug.as_debug_string(funcsig_call)}]")
            # See if this is a virtual function that isn't defined for this class, and add a using
            # statement for it...

            mismatch_found = False
            for virt_func in self._code_elements.virtual_member_functions:
                debug.line("post_process_function_calls", f"CHECKING VIRT_FUNC funcsig=[{debug.as_debug_string(virt_func.funcsig)}]")

                if funcsig_call.name == virt_func.funcsig.name:
                    debug.line("post_process_function_calls", f"NAME MATCH -> funcsig_call=[{debug.as_debug_string(funcsig_call)}] virt_func.funcsig=[{debug.as_debug_string(virt_func.funcsig)}]")

                    # Get just the valid (not NONE_VALUE) args for the virtual function under test
                    virt_func_args = [a for a in virt_func.funcsig.args if a != NONE_VALUE]

                    for i in range(len(funcsig_call.args)):
                        if funcsig_call.args[i].decl_spec.type != virt_func_args[i].decl_spec.type:
                            # MISMATCH: Add base function name
                            debug.line("post_process_function_calls", f"MISMATCH FOUND, adding base function name=[{funcsig_call.name}]: i=[{i}] funcsig_call.args[i]=[{debug.as_debug_string(funcsig_call.args[i])}] virt_func_args[i]=[{debug.as_debug_string(virt_func_args[i])}]")
                            self._code_info.add_base_function_name(funcsig_call.name)
                            mismatch_found = True
                            break
                
                if mismatch_found:
                    break
