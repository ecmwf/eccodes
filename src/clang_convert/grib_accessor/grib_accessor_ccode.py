
import debug
import default.ccode as ccode
import cfunction
import re

grib_accessor_inherited_methods = [
    "dump",
    "next_offset",
    "string_length",
    "value_count",
    "byte_count",
    "byte_offset",
    "get_native_type",
    "sub_section",
    "pack_missing",
    "is_missing",
    "pack_long",
    "unpack_long",
    "pack_double",
    "pack_float",
    "unpack_double",
    "unpack_float",
    "pack_string",
    "unpack_string",
    "pack_string_array",
    "unpack_string_array",
    "pack_bytes",
    "unpack_bytes",
    "pack_expression",
    "notify_change",
    "update_size",
    "preferred_size",
    "resize",
    "nearest_smaller_value",
    "next",
    "compare",
    "unpack_double_element",
    "unpack_float_element",
    "unpack_double_element",
    "unpack_float_element",
    "unpack_double_subarray",
    "clear",
    "clone",
]

# Represents a grib_accessor_class_*cc file
class GribAccessorCCode(ccode.CCode):
    def __init__(self, cfilename, accessor_name, accessor_class_name) -> None:
        super().__init__(cfilename)
        self._accessor_name = accessor_name
        self._accessor_class_name = accessor_class_name
        self._super_class_name = ""
        self._accessor_class_short_name = ""
        self._members = []
        self._inherited_methods = []
        self._private_methods = []
        self._static_functions = []
        self._constructor = None
        self._destructor = None

    @property
    def parent_cfilename(self):
        if self._super_class_name:
            return self._super_class_name + ".cc"
        else:
            return super().parent_cfilename


    def add_member(self, cmember):
        assert cmember not in self._members, f"member [{cmember.as_string()}] already defined for [{self._accessor_class_name}]"
        self._members.append(cmember)

    # Overridden to classify the functions being added
    def add_function(self, cfuncsig, body):
        cfunc = cfunction.CFunction(cfuncsig, body)

        if cfuncsig.name == "init":
            self._constructor = cfunc
            return
        elif cfuncsig.name == "destroy":
            self._destructor = cfunc
            return
        elif cfuncsig.name in grib_accessor_inherited_methods:
            self._inherited_methods.append(cfunc)
            return
        
        # If any arg starts with a "ptr type name", then it's a private method (as we've already extracted inherited functions)
        for arg_entry in cfuncsig.args:
            if re.search(r"grib_accessor(\w*)?\s*\*", arg_entry.decl_spec.as_string()):
                self._private_methods.append(cfunc)
                return

        # Must be a static function
        self._static_functions.append(cfunc)

    def dump_class_info(self):
        debug.line("dump_class_info", f"accessor_name=[{self._accessor_name}]")
        debug.line("dump_class_info", f"accessor_class_name=[{self._accessor_class_name}]")
        debug.line("dump_class_info", f"accessor_class_short_name=[{self._accessor_class_short_name}]")
        debug.line("dump_class_info", f"super_class_name=[{self._super_class_name}]")

    def dump_members(self):
        for cmember in self._members:
            debug.line("dump_members", f"member=[{cmember.as_string()}]")

    def dump_functions(self):
        debug.line("dump_functions", f"Constructor=[{self._constructor.funcsig.as_string() if self._constructor else None}]")
        debug.line("dump_functions", f"Destructor=[{self._destructor.funcsig.as_string() if self._destructor else None}]")

        for entry in self._inherited_methods:
            debug.line("dump_functions", f"Inherited=[{entry.funcsig.as_string()}]")
        for entry in self._private_methods:
            debug.line("dump_functions", f"Private=[{entry.funcsig.as_string()}]")
        for entry in self._static_functions:
            debug.line("dump_functions", f"Static=[{entry.funcsig.as_string()}]")


    def dump(self):
        self.dump_class_info()
        self.dump_members()
        self.dump_global_declarations()
        self.dump_functions()