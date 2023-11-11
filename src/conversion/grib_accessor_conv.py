import debug
from accessor_data import AccessorData
import arg
import arg_conv
import member_conv
from converter_collection import Converter, default_converters
import transforms
from funcsig_mapping import FuncSigMapping
import grib_api_converter
import copy
import importlib
import accessor_specific.default

prefix = "grib_accessor_class_"
rename = {
    "Gen": "Accessor",      # "Generic",
    "Md5": "Md5Sum",        # We rename because of macos case insensitive file system
    "Assert": "Assertion",  # Name clash with assert.h
}

common_includes = [
    "Accessor.h", 
    "AccessorFactory.h", 
    "AccessorUtils/ConversionHelper.h",
    "AccessorUtils/AccessorException.h",
    "GribCpp/GribCppIncludes.h",
    "GribStub/GribStubIncludes.h",
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

# These will be used if no other supplied...
common_funcsig_type_transforms = {
    "char*"   : "std::string&",
    "char[]"  : "std::string&",
}

# These will be used if no other supplied...
common_type_transforms = {
    "char**"            : "std::string&",
    "char*"             : "std::string",
    "char[]"            : "std::string",
    "grib_expression*"  : "GribExpressionPtr"
}

# Define base class member mapping
base_members_map = {
    arg.Arg("long","length") : arg.Arg("long","length_"),
    arg.Arg("long","offset") : arg.Arg("long","offset_"),
    arg.Arg("unsigned long","flags") : arg.Arg("unsigned long","flags_"),
    arg.Arg("int","dirty") : arg.Arg("int","dirty_"),
    arg.Arg("grib_virtual_value*","vvalue") : arg.Arg("std::unique_ptr<grib_virtual_value>","vvalue_"),
    arg.Arg("const char*","set") : arg.Arg("std::string","set_")
    }

# Convert GribAccessor to AccessorData
class GribAccessorConverter:
    def __init__(self, grib_accessor_instance):
        self._grib_accessor = grib_accessor_instance
        self._accessor_data = None
        self._accessor_data_name = self.transform_class_name(self._grib_accessor.name)
        self._transforms = None

    # Convert GribAccessor to AccessorData
    def to_accessor_data(self, other_grib_accessors):
        debug.line("to_accessor_data", f"\n===== [CONVERTING:BEGIN] {self._grib_accessor.name} ====================\n")

        self.setup_accessor_specific()
        self._converters = copy.deepcopy(default_converters)
        self._converters = self._accessor_specific.update_converters(self._converters)

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
        self.create_funcsig_mappings()
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

    def setup_accessor_specific(self):
        self._accessor_specific = None

        module_name = f"accessor_specific.{self._grib_accessor.name}"
        class_name = f"{self._accessor_data_name}AccessorSpecific"

        debug.line("setup_accessor_specific", f"Looking for [{class_name}] in module [{module_name}]")
        try:
            m = importlib.import_module(module_name)

            if class_name in dir(m):
                AccessorSpecificClass = getattr(m, class_name)
                self._accessor_specific = AccessorSpecificClass()
                assert self._accessor_specific, f"AccessorSpecific inst for {self._grib_accessor.name} is None"
                debug.line("setup_accessor_specific", f"Loaded [{class_name}]")
        except ModuleNotFoundError:
            pass

        if not self._accessor_specific:
            self._accessor_specific = accessor_specific.default.AccessorSpecific()
            debug.line("setup_accessor_specific", f"[{class_name}] not found... loaded [AccessorSpecific] (default) instead")

    def create_accessor_data(self):
        cpp_super_name = self.transform_class_name(self._grib_accessor.super)
        # Need to keep the C name for the factory as it is called from C code...
        cpp_factory_name = self._grib_accessor.factory_name

        self._accessor_data = AccessorData(self._accessor_data_name, cpp_super_name, cpp_factory_name)

    # Get members all the way up the hierarchy
    def members_in_hierarchy(self, grib_accessor_inst):
        result = set()
        for m in grib_accessor_inst.members:
            result.add(m)
        if grib_accessor_inst.super in self.other_grib_accessors:
            result.update(self.members_in_hierarchy(self.other_grib_accessors[grib_accessor_inst.super]))
        return result

    def create_transforms(self):
        funcsig_type_transforms = copy.deepcopy(common_funcsig_type_transforms)
        funcsig_type_transforms.update(grib_api_converter.grib_api_funcsig_type_transforms())

        for k,v in funcsig_type_transforms.items():
            debug.line("create_transforms",f"Funcsig type transform: {k} -> {v}")

        type_transforms = copy.deepcopy(common_type_transforms)
        type_transforms.update(grib_api_converter.grib_api_type_transforms())

        for k,v in type_transforms.items():
            debug.line("create_transforms",f"Type transform: {k} -> {v}")

        self._transforms = transforms.Transforms(funcsig_types=funcsig_type_transforms, types=type_transforms)

        self._transforms.add_to_types("self", self._accessor_data.name)
        self._transforms.add_to_types("super", self._accessor_data.super)

        # There are some instances of accessing super->super, so we'll store this as well!
        cpp_super_super_name = None

        if self._grib_accessor.super in self.other_grib_accessors.keys():
            cpp_super_super_name = self.transform_class_name(self.other_grib_accessors[self._grib_accessor.super].super)

        self._transforms.add_to_types("supersuper", cpp_super_super_name)

        # Add in any custom transforms
        self._transforms = self._accessor_specific.add_custom_transforms(self._transforms)

    def add_global_function(self):
        global_func_funcsig_converter = self._converters[Converter.GLOBAL_FUNC_FUNCSIG](self._grib_accessor.global_function.func_sig)
        mapping = global_func_funcsig_converter.create_funcsig_mapping(self._transforms)
        self._transforms.add_to_other_funcsig_mappings(mapping)

        static_func_name_transforms = {}
        for func in self._grib_accessor.static_functions:
            converter = self._converters[Converter.STATIC_FUNC_FUNCSIG](func.func_sig)
            cppfuncname = converter.to_cpp_name()
            static_func_name_transforms[func.name] = cppfuncname

        self._global_func_converter = self._converters[Converter.GLOBAL_FUNC](static_func_name_transforms)
        self._accessor_data.global_function = self._global_func_converter.to_cpp_function(self._grib_accessor.global_function, self._transforms)
        self._transforms.make_global()

    def create_funcsig_mappings(self):
        # Create funcsig mappings for all C funcs, and add to transforms 
        for func in self._grib_accessor.inherited_methods:
            converter = self._converters[Converter.INHERITED_METHOD_FUNCSIG](func.func_sig)
            mapping = converter.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_inherited_funcsig_mappings(mapping)

        for func in self._grib_accessor.private_methods:
            converter = self._converters[Converter.PRIVATE_METHOD_FUNCSIG](func.func_sig)
            mapping = converter.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_private_funcsig_mappings(mapping)

        for func in self._grib_accessor.static_functions:
            converter = self._converters[Converter.STATIC_FUNC_FUNCSIG](func.func_sig)
            mapping = converter.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_static_funcsig_mappings(mapping)

        # Add "other" funcsigs
        other_funcs = {
            self._grib_accessor.constructor: self._converters[Converter.CONSTRUCTOR_METHOD_FUNCSIG],
            self._grib_accessor.destructor: self._converters[Converter.DESTRUCTOR_METHOD_FUNCSIG]
        }

        for func, conv in other_funcs.items():
            converter = conv(func.func_sig)
            mapping = converter.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_other_funcsig_mappings(mapping)

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
        # Base members
        for cmember, cppmember in base_members_map.items():
            self._transforms.add_to_members(cmember, cppmember)

        for cmember in self.members_in_hierarchy(self._grib_accessor):
            member_converter = member_conv.MemberConverter(cmember)
            cppmember = member_converter.to_cpp_arg(self._transforms)
            if cmember in self._grib_accessor.members:
                self._accessor_data.add_member(cppmember)
            self._transforms.add_to_members(cmember, cppmember)

    def add_constructor_method(self):
        # Create a default constructor if none exists
        if self._grib_accessor.constructor:
            constructor_method_converter = self._converters[Converter.CONSTRUCTOR_METHOD]()
            self._accessor_data._constructor = constructor_method_converter.to_cpp_function(self._grib_accessor.constructor, self._transforms)
        else:
            self._accessor_data._constructor = self._converters[Converter.CONSTRUCTOR_METHOD]()

    def add_destructor_method(self):
        # Create a default destructor if none exists
        if self._grib_accessor.destructor:
            destructor_method_converter = self._converters[Converter.DESTRUCTOR_METHOD]()
            self._accessor_data._destructor = destructor_method_converter.to_cpp_function(self._grib_accessor.destructor, self._transforms)
        else:
            self._accessor_data._destructor = self._converters[Converter.DESTRUCTOR_METHOD]()

    def add_inherited_methods(self):
        for cfunc in self._grib_accessor.inherited_methods:
            inherited_method_converter = self._converters[Converter.INHERITED_METHOD]()
            cppfunc = inherited_method_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                cppfunc.const = cfunc.name not in non_const_cmethods
                self._accessor_data.add_inherited_method(cppfunc)

    def add_private_methods(self):
        for cfunc in self._grib_accessor.private_methods:
            private_method_converter = self._converters[Converter.PRIVATE_METHOD]()
            cppfunc = private_method_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                self._accessor_data.add_private_method(cppfunc)

    def add_static_functions(self):
        for cfunc in self._grib_accessor.static_functions:
            static_function_converter = self._converters[Converter.STATIC_FUNC]()
            cppfunc = static_function_converter.to_cpp_function(cfunc, self._transforms)
            if cppfunc.name is not None:
                self._accessor_data.add_static_function(cppfunc)

    # Search for argument types used in private methods where the type is defined in the .cc
    # file and so needs a forward declaration in the header
    def add_forward_declarations(self):
        # [1] Update the placeholders in the global function
        self._global_func_converter.process_forward_declarations(self._transforms.static_funcsig_mappings)

        # [2] Find and add forward declarations to the class header
        for global_arg in self._transforms.global_args.values():
            if not global_arg:
                continue

            for mapping in self._transforms.private_funcsig_mappings:
                for cpp_arg in mapping.cppfuncsig.args:
                    if cpp_arg and cpp_arg.underlying_type == global_arg.name:
                        debug.line("add_forward_declarations", f"Found a forward declaration: Method: {mapping.cppfuncsig.name} arg: {arg.arg_string(cpp_arg)} declaration: {arg.arg_string(global_arg)}")
                        self._accessor_data.add_forward_declaration(arg.arg_string(global_arg)+";")


    # Convert e.g. grib_accessor_class_proj_string to ProjString
    def transform_class_name(self, name):
        name = name.replace(prefix, "")
        name = arg_conv.transform_type_name(name)
        return rename.get(name, name) + "Data"
