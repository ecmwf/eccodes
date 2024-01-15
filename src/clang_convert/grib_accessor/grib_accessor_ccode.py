
import debug
import default.default_ccode as default_ccode
import code_object.cfunction as cfunction
import re

grib_accessor_virtual_member_functions = [
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
class GribAccessorCCode(default_ccode.DefaultCCode):
    def __init__(self, cfilename, accessor_name, accessor_class_name) -> None:
        super().__init__(cfilename)
        self._accessor_name = accessor_name
        self._class_name = accessor_class_name
        self._accessor_class_short_name = ""

    @property
    def parent_cfilename(self):
        if self._super_class_name:
            return self._super_class_name + ".cc"
        else:
            return super().parent_cfilename

    # Overridden to classify the functions being added
    def add_function(self, cfuncsig, body):
        cfunc = cfunction.CFunction(cfuncsig, body)

        if cfuncsig.name == "init":
            self._constructor = cfunc
            return
        elif cfuncsig.name == "destroy":
            self._destructor = cfunc
            return
        elif cfuncsig.name in grib_accessor_virtual_member_functions:
            self._virtual_member_functions.append(cfunc)
            return
        
        # If any arg starts with a "ptr type name", then it's a member function (as we've already extracted virtual functions)
        for arg_entry in cfuncsig.args:
            if re.search(r"grib_accessor(\w*)?\s*\*", arg_entry.decl_spec.as_string()):
                self._member_functions.append(cfunc)
                return

        # Must be a "free"" function
        self._functions.append(cfunc)

    def dump_class_info(self):
        super().dump_class_info()
        debug.line("dump_class_info", f"accessor_name=[{self._accessor_name}]")
        debug.line("dump_class_info", f"accessor_class_short_name=[{self._accessor_class_short_name}]")
