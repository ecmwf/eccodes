
import utils.debug as debug
import default.default_ccode as default_ccode
import grib_accessor.supporting.virtual_member_functions as virtual_member_functions
import re

# Represents a grib_accessor_class_*cc file
class GribAccessorCCode(default_ccode.DefaultCCode):
    def __init__(self, cfilename, accessor_name, accessor_class_name) -> None:
        super().__init__(cfilename)
        self._accessor_name = accessor_name
        self._class_name = accessor_class_name
        self._accessor_class_short_name = ""

    @property
    def accessor_name(self):
        return self._accessor_name

    @property
    def parent_cfilename(self):
        if self._super_class_name:
            return self._super_class_name + ".cc"
        else:
            return super().parent_cfilename

    def is_constructor(self, cfuncsig):
        return cfuncsig.name == "init"

    def is_destructor(self, cfuncsig):
        return cfuncsig.name == "destroy"

    def is_virtual_member_function(self, cfuncsig):
        return cfuncsig.name in virtual_member_functions.virtual_member_function_names

    def is_member_function(self, cfuncsig):
        if not self.is_virtual_member_function(cfuncsig):
            # If first arg starts with a "ptr type name", then it's a member function (as we've already extracted virtual functions)
            if len(cfuncsig.args) > 0:
                if re.search(r"grib_accessor(\w*)?\s*\*", cfuncsig.args[0].decl_spec.as_string()):
                    return True

        return False

    def dump_class_info(self):
        super().dump_class_info()
        debug.line("dump_class_info", f"accessor_name=[{self._accessor_name}]")
        debug.line("dump_class_info", f"accessor_class_short_name=[{self._accessor_class_short_name}]")