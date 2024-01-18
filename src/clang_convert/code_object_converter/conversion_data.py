
import utils.debug as debug
from code_object.code_interface import as_debug_string

# Store C to C++ conversion data to be used by the converters
#
# The converters will add/update the entries here as required
class ConversionData:
    def __init__(self, *, funcsig_decl_specs={}, decl_specs={}, ) -> None:
        self._funcsig_decl_specs = funcsig_decl_specs
        self._decl_specs = decl_specs
        self._custom_args = {}
        self._global_args = {}
        self._local_args = {}   # Local to the function being processed
        self._funcsig_mappings = []
        self._funcsig_pointer_mappings = []

    @property
    def funcsig_decl_specs(self):
        return self._funcsig_decl_specs

    def add_to_funcsig_decl_specs(self, cdecl_spec, cppdecl_spec):
        if cdecl_spec in self._funcsig_decl_specs:
            assert self._funcsig_decl_specs[cdecl_spec] == cppdecl_spec, f"Updating an existing funcsig decl spec: [{cdecl_spec.as_string()}] -> [{cppdecl_spec.as_string()}] Previous decl spec=[{self._types[cdecl_spec].as_string()}]"
        else:
            self._funcsig_decl_specs[cdecl_spec] == cppdecl_spec
            debug.line("ConversionData", f"Adding decl spec: [{cdecl_spec.as_string()}] -> [{cppdecl_spec.as_string()}]")

    @property
    def decl_specs(self):
        return self._decl_specs

    def add_to_decl_spec(self, cdecl_spec, cppdecl_spec):
        if cdecl_spec in self._decl_specs:
            assert self._decl_specs[cdecl_spec] == cppdecl_spec, f"Updating an existing decl spec mapping: [{cdecl_spec.as_string()}] -> [{cppdecl_spec.as_string()}] Previous decl spec=[{self._types[cdecl_spec].as_string()}]"
        else:
            self._decl_specs[cdecl_spec] == cppdecl_spec
            debug.line("ConversionData", f"Adding decl spec mapping: [{cdecl_spec.as_string()}] -> [{cppdecl_spec.as_string()}]")

    @property
    def custom_args(self):
        return self._custom_args

    def add_to_custom_args(self, carg, cpparg):
        if carg in self._custom_args:
            assert self._custom_args[carg] == cpparg, f"Updating an existing custom arg mapping: C Arg = {as_debug_string(carg)} -> {as_debug_string(cpparg)} Previous arg = {as_debug_string(self._all_args[carg])}"
        else:
            assert carg, f"ADDING carg which is None!"
            debug.line("ConversionData", f"Adding new custom arg mapping: {as_debug_string(carg)} -> {as_debug_string(cpparg)}")
            self._custom_args[carg] = cpparg

    @property
    def global_args(self):
        return self._global_args

    def add_to_global_args(self, carg, cpparg):
        if carg in self._global_args:
            assert self._global_args[carg] == cpparg, f"Updating an existing global arg mapping: C Arg = {as_debug_string(carg)} -> {as_debug_string(cpparg)} Previous arg = {as_debug_string(self._all_args[carg])}"
        else:
            assert carg, f"ADDING carg which is None!"
            debug.line("ConversionData", f"Adding new global arg mapping: {as_debug_string(carg)} -> {as_debug_string(cpparg)}")
            self._global_args[carg] = cpparg

    @property
    def local_args(self):
        return self._local_args

    def add_to_local_args(self, carg, cpparg):
        if carg in self._local_args:
            assert self._local_args[carg] == cpparg, f"Updating an existing local arg mapping: C Arg = {as_debug_string(carg)} -> {as_debug_string(cpparg)} Previous arg = {as_debug_string(self._all_args[carg])}"
        else:
            assert carg, f"ADDING carg which is None!"
            debug.line("ConversionData", f"Adding new local arg mapping: {as_debug_string(carg)} -> {as_debug_string(cpparg)}")
            self._local_args[carg] = cpparg

    @property
    def funcsig_mappings(self):
        return self._funcsig_mappings

    # Clear current mappings and set to supplied mappings
    def set_funcsig_mappings(self, new_mappings):
        self._funcsig_mappings.clear()
        self._funcsig_mappings.extend(new_mappings)

    def add_to_funcsig_mappings(self, mapping):
        for entry in self._funcsig_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"

        self._funcsig_mappings.append(mapping)

    def cppfuncsig_for_cfuncsig(self, cfuncsig):
        for mapping in self._funcsig_mappings:
            if cfuncsig.name == mapping.cfuncsig.name:
                return mapping.cppfuncsig
        
        return None

    def cppfuncsig_for_cfuncname(self, cfuncname):
        for mapping in self._funcsig_mappings:
            if cfuncname == mapping.cfuncsig.name:
                return mapping.cppfuncsig
        
        return None

    @property
    def funcsig_pointer_mappings(self):
        return self._funcsig_pointer_mappings

    def add_to_funcsig_pointer_mappings(self, mapping):
        for entry in self._funcsig_pointer_mappings:
            if entry.cfuncsig_pointer.name == mapping.cfuncsig_pointer.name:
                assert False, f"Mapping for [{mapping.cfuncsig_pointer.name}] already exists!"

        self._funcsig_pointer_mappings.append(mapping)

    def cppfuncsig_pointer_for_cfuncsig_pointer(self, cfuncsig_pointer):
        for mapping in self._funcsig_pointer_mappings:
            if cfuncsig_pointer.name == mapping.cfuncsig_pointer.name:
                return mapping.cppfuncsig_pointer
        
        return None
