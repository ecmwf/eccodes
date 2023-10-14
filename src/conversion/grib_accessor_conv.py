import debug
from grib_accessor import GribAccessor
from accessor_data import AccessorData
import arg
import arg_conv
import member_conv
import global_func_conv
import global_func_funcsig_conv
import constructor_method
import constructor_method_conv
import constructor_method_funcsig_conv
import funcsig
import destructor_method
import destructor_method_conv
import destructor_method_funcsig_conv
import inherited_method_conv
import inherited_method_funcsig_conv
import private_method_conv
import private_method_funcsig_conv
import static_func_conv
import static_func_funcsig_conv
import transforms
import re

prefix = "grib_accessor_class_"
rename = {
    "Gen": "Accessor",      # "Generic",
    "Md5": "Md5Sum",        # We rename because of macos case insensitive file system
    "Assert": "Assertion",  # Name clash with assert.h
}

common_includes = [
    "AccessorFactory.h", 
    "AccessorUtils/ConversionHelper.h",
    "AccessorUtils/GribUtils.h",
    "AccessorUtils/GribAccessorFlag.h",
    "AccessorUtils/AccessorException.h"
]

non_const_cmethods = [
    "pack_long",
    "pack_missing",
    "pack_string",
    "pack_double",
    "pack_bytes",
    "pack_expression",
    "pack_string_array",
    "update_size",
    "notify_change",
]

# Convert GribAccessor to AccessorData
class GribAccessorConverter:
    def __init__(self, grib_accessor_instance):
        self._grib_accessor = grib_accessor_instance
        self._accessor_data = None
        self._transforms = None

    # Convert GribAccessor to AccessorData
    def to_accessor_data(self, other_grib_accessors):
        debug.line("to_accessor_data", f"\n===== [CONVERTING:BEGIN] {self._grib_accessor.name} ====================\n")

        # Note: we have a chicken and egg scenario when converting the C code:
        # - Some classes (e.g. bufr_data_array) need the global vars to be parsed BEFORE the funcsig transforms because
        #   the converted vars (e.g. struct bitmap_s) need to be available for function arguments
        # - Other classes (e.g. proj_string) need the funcsig transforms to be parsed BEFORE the global vars because 
        #   the global proj_mappings[] array needs the C++ function names!
        #
        # The solution is to have a two-phase parse of the global object, using the following steps:
        # 1. [add_global_function] Create transforms for just the static function names (ignoring the rest of the signature)
        # 2. [add_global_function] Parse the global function to map all C-C++ global vars and substitute any C func calls with
        #    their C++ equivalent
        #    During this step, forward declarations are ignored
        # 3. [add_funcsig_transforms] Create full funcsig transforms for all functions, using the global var transforms from
        #    step 2 where required
        # 4. [add_forward_declarations] Once everything else has been converted, run a second pass of the global function and
        #    add all forward declarations in place of the placeholder text
        # 5. [add_forward_declarations] Determine any global vars that are being used as function args in the new class member
        #    functions and add a forward declaration of these in the header file
        self.other_grib_accessors = other_grib_accessors
        self.create_accessor_data()
        self.create_transforms()
        self.add_global_function()
        self.add_funcsig_transforms()
        self.add_includes()
        self.add_members()
        self.add_constructor_method()
        self.add_destructor_method()
        self.add_inherited_methods()
        self.add_private_methods()
        self.add_static_functions()
        self.add_forward_declarations()

        debug.line("to_accessor_data", f"\n===== [CONVERTING:END] {self._grib_accessor.name} ====================\n")

        return self._accessor_data

    def create_accessor_data(self):
        cpp_name = self.transform_class_name(self._grib_accessor.name)
        cpp_super_name = self.transform_class_name(self._grib_accessor.super)
        # Need to keep the C name for the factory as it is called from C code...
        cpp_factory_name = self._grib_accessor.factory_name

        self._accessor_data = AccessorData(cpp_name, cpp_super_name, cpp_factory_name)

    # Get members all the way up the hierarchy
    def members_in_hierarchy(self, grib_accessor_inst):
        result = set()
        for m in grib_accessor_inst.members:
            result.add(m)
        if grib_accessor_inst.super in self.other_grib_accessors:
            result.update(self.members_in_hierarchy(self.other_grib_accessors[grib_accessor_inst.super]))
        return result

    def create_transforms(self):
        self._transforms = transforms.Transforms(types=arg_conv.common_type_transforms)

        self._transforms.add_to_class_types("self", self._grib_accessor.name, self._accessor_data.name)
        self._transforms.add_to_class_types("super", self._grib_accessor.super, self._accessor_data.super)

    def add_global_function(self):
        global_func_funcsig_converter = global_func_funcsig_conv.GlobalFunctionFuncSigConverter(self._grib_accessor.global_function.func_sig)
        cfuncsig, cppfuncsig = global_func_funcsig_converter.to_cpp_funcsig()
        self._transforms.add_to_other_funcsigs(cfuncsig, cppfuncsig)

        static_func_name_transforms = {}
        for func in self._grib_accessor.static_functions:
            converter = static_func_funcsig_conv.StaticFunctionFuncSigConverter(func.func_sig)
            cppfuncname = converter.to_cpp_name()
            static_func_name_transforms[func.name] = cppfuncname

        self._global_func_converter = global_func_conv.GlobalFunctionConverter(static_func_name_transforms)
        self._accessor_data.global_function = self._global_func_converter.to_cpp_function(self._grib_accessor.global_function, self._transforms)
        self._transforms.make_global()

    def add_funcsig_transforms(self):
        # Create funcsig transforms for all C funcs
        for func in self._grib_accessor.inherited_methods:
            converter = inherited_method_funcsig_conv.InheritedMethodFuncSigConverter(func.func_sig)
            cfuncsig, cppfuncsig = converter.to_cpp_funcsig()
            self._transforms.add_to_inherited_funcsigs(cfuncsig, cppfuncsig)

        for func in self._grib_accessor.private_methods:
            converter = private_method_funcsig_conv.PrivateMethodFuncSigConverter(func.func_sig)
            cfuncsig, cppfuncsig = converter.to_cpp_funcsig()
            self._transforms.add_to_private_funcsigs(cfuncsig, cppfuncsig)

        for func in self._grib_accessor.static_functions:
            converter = static_func_funcsig_conv.StaticFunctionFuncSigConverter(func.func_sig)
            cfuncsig, cppfuncsig = converter.to_cpp_funcsig()
            self._transforms.add_to_static_funcsigs(cfuncsig, cppfuncsig)

        # Add "other" funcsigs
        other_funcs = {
            self._grib_accessor.constructor: constructor_method_funcsig_conv.ConstructorMethodFuncSigConverter,
            self._grib_accessor.destructor: destructor_method_funcsig_conv.DestructorMethodFuncSigConverter
        }

        for func, conv in other_funcs.items():
            converter = conv(func.func_sig)
            cfuncsig, cppfuncsig = converter.to_cpp_funcsig()
            self._transforms.add_to_other_funcsigs(cfuncsig, cppfuncsig)

    def add_includes(self):
        # Header includes
        if self._accessor_data.super == "AccessorData":
            self._accessor_data.add_header_include("/".join(["AccessorData", self._accessor_data.super + ".h"]))
        else:
            self._accessor_data.add_header_include(f"{self._accessor_data.super}.h")

        # Source includes
        self._accessor_data.add_source_include(f"{self._accessor_data.name}.h")
        
        for inc in common_includes:
            self._accessor_data.add_source_include(inc)

    # Adds all members for this accessor to the accessor_data object,
    # And also stores ALL members in the hierarchy in the transform
    def add_members(self):
        for cmember in self.members_in_hierarchy(self._grib_accessor):
            member_converter = member_conv.MemberConverter(cmember)
            cppmember = member_converter.to_cpp_arg()
            if cmember in self._grib_accessor.members:
                self._accessor_data.add_member(cppmember)
            self._transforms.add_to_members(cmember, cppmember)

    def add_constructor_method(self):
        # Create a default constructor if none exists
        if self._grib_accessor.constructor:
            constructor_method_converter = constructor_method_conv.ConstructorMethodConverter()
            self._accessor_data._constructor = constructor_method_converter.to_cpp_function(self._grib_accessor.constructor, self._transforms)
        else:
            self._accessor_data._constructor = constructor_method.ConstructorMethod()

    def add_destructor_method(self):
        # Create a default destructor if none exists
        if self._grib_accessor.destructor:
            destructor_method_converter = destructor_method_conv.DestructorMethodConverter()
            self._accessor_data._destructor = destructor_method_converter.to_cpp_function(self._grib_accessor.destructor, self._transforms)
        else:
            self._accessor_data._destructor = destructor_method.DestructorMethod()

    def add_inherited_methods(self):
        for cfunc in self._grib_accessor.inherited_methods:
            inherited_method_converter = inherited_method_conv.InheritedMethodConverter()
            cppfunc = inherited_method_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                cppfunc.const = cfunc.name not in non_const_cmethods
                self._accessor_data.add_inherited_method(cppfunc)

    def add_private_methods(self):
        for cfunc in self._grib_accessor.private_methods:
            private_method_converter = private_method_conv.PrivateMethodConverter()
            cppfunc = private_method_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                cppfunc.const = cfunc.name not in non_const_cmethods
                self._accessor_data.add_private_method(cppfunc)

    def add_static_functions(self):
        for cfunc in self._grib_accessor.static_functions:
            static_function_converter = static_func_conv.StaticFunctionConverter()
            cppfunc = static_function_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                self._accessor_data.add_static_function(cppfunc)

    # Search for argument types used in private methods where the type is defined in the .cc
    # file and so needs a forward declaration in the header
    def add_forward_declarations(self):
        # [1] Update the placeholders in the global function
        self._global_func_converter.process_forward_declarations(self._transforms.static_funcsigs)

        # [2] Find and add forward declarations to the class header
        for global_arg in self._transforms.global_args.values():
            if not global_arg:
                continue

            for cppfuncsig in self._transforms.private_funcsigs.values():
                for cpp_arg in cppfuncsig.args:
                    if cpp_arg and cpp_arg.underlying_type == global_arg.name:
                        debug.line("add_forward_declarations", f"Found a forward declaration: Method: {cppfuncsig.name} arg: {arg.arg_string(cpp_arg)} declaration: {arg.arg_string(global_arg)}")
                        self._accessor_data.add_forward_declaration(arg.arg_string(global_arg)+";")


    # Convert e.g. grib_accessor_class_proj_string to ProjString
    def transform_class_name(self, name):
        name = name.replace(prefix, "")
        name = arg_conv.transform_type_name(name)
        return rename.get(name, name) + "Data"