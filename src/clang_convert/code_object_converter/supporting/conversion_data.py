
from enum import Enum, auto
import utils.debug as debug
import cpp_code.code_info as code_info
import code_object_converter.supporting.code_mappings as code_mappings
import code_object_converter.supporting.funcsig_mapping as funcsig_mapping
import code_object_converter.supporting.funcsig_pointer_mapping as funcsig_pointer_mapping

# Store C to C++ conversion data to be used by the converters
#
# The converters will add/update the entries here as required
#

# Set the current state of the class:
# GLOBAL    - any new data is added to the global section (i.e. global function)
# LOCAL     - any new data is added to the local section, and can be erased (i.e. function-specific)
class ConversionDataState(Enum):
    GLOBAL = auto()
    LOCAL = auto()


class ConversionData:
    def __init__(self, info) -> None:
        self._state = ConversionDataState.GLOBAL
        self._info = info
        self._global_mappings = code_mappings.CodeMappings()
        self._local_mappings = None

    # Call this to ensure local state is set ready for function conversions
    def set_local_state(self):
        self._state = ConversionDataState.LOCAL
        self.reset_local_state()

    # Clear out / reset local state data ready to convert a new function
    def reset_local_state(self):
        pass

    # The info object can be manipulated directly
    @property
    def info(self):
        return self._info

    # ============================== Functions to update the mappings: start ==============================

    @property
    def active_map(self):
        if self._state == ConversionDataState.LOCAL:
            return self._local_mappings
        else:
            return self._global_mappings

    def add_arg_mapping(self, carg, cpparg):
        if carg in self.active_map.arg_mappings:
            assert self.active_map.arg_mappings[carg] == cpparg, f"Updating an existing arg: [{carg.as_string()}] -> [{cpparg.as_string()}] Previous arg=[{self.active_map.arg_mappings[carg]}]"
        else:
            self.active_map.arg_mappings[carg] == cpparg
            debug.line("add_arg_mapping", f"Adding arg: [{carg.as_string()}] -> [{cpparg.as_string()}]")

    def add_function_arg_mapping(self, carg, cpparg):
        if carg in self.active_map.function_arg_mappings:
            assert self.active_map.function_arg_mappings[carg] == cpparg, f"Updating an existing function arg: [{carg.as_string()}] -> [{cpparg.as_string()}] Previous function arg=[{self.active_map.function_arg_mappings[carg]}]"
        else:
            self.active_map.function_arg_mappings[carg] == cpparg
            debug.line("add_function_arg_mappings", f"Adding function arg: [{carg.as_string()}] -> [{cpparg.as_string()}]")

    def add_funcsig_mapping(self, mapping):
        assert isinstance(mapping, funcsig_mapping.FuncSigMapping), f"Expected FuncSigMapping, got type=[{type(mapping).__name__}]"
        for entry in self.active_map.funcsig_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"

        self.active_map.funcsig_mappings.append(mapping)

    def add_funcsig_pointer_mapping(self, mapping):
        assert isinstance(mapping, funcsig_pointer_mapping.FuncSigPointerMapping), f"Expected FuncSigPointerMapping, got type=[{type(mapping).__name__}]"
        for entry in self.active_map.funcsig_pointer_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"
        
        self.active_map.funcsig_pointer_mappings.append(mapping)

    def add_member_funcsig_mapping(self, mapping):
        assert isinstance(mapping, funcsig_mapping.FuncSigMapping), f"Expected FuncSigMapping, got type=[{type(mapping).__name__}]"
        for entry in self.active_map.member_funcsig_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"

        self.active_map.member_funcsig_mappings.append(mapping)

    def add_virtual_member_funcsig_mapping(self, mapping):
        assert isinstance(mapping, funcsig_mapping.FuncSigMapping), f"Expected FuncSigMapping, got type=[{type(mapping).__name__}]"
        for entry in self.active_map.virtual_member_funcsig_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"
        
        self.active_map.virtual_member_funcsig_mappings.append(mapping)

    # ============================== Functions to update the mappings: end   ==============================
    
    # ============================== Functions to query the mappings:  start ==============================

    def all_mappings(self):
        if self._local_mappings:
            return [self._local_mappings, self._global_mappings]
        else:
            return [self._global_mappings]

    def cpparg_for_carg(self, carg):
        for mapping in self.all_mappings():
            for entry in mapping.arg_mappings:
                if entry.name == carg.name:
                    return entry.cpp_arg
        return None

    def cppfunction_arg_for_carg(self, carg):
        for mapping in self.all_mappings():
            for entry in mapping.function_arg_mappings:
                if entry.name == carg.name:
                    return entry.cpp_arg
        return None

    def cppfuncsig_for_cfuncsig(self, cfuncsig):
        for mapping in self.all_mappings():
            for entry in mapping.all_funcsig_mappings:
                if entry.cfuncsig.name == cfuncsig.name:
                    return entry.cppfuncsig
        return None

    def cppfuncsig_for_cfuncname(self, cfuncname):
        for mapping in self.all_mappings():
            for entry in mapping.all_funcsig_mappings:
                if entry.cfuncsig.name == cfuncname:
                    return entry.cppfuncsig
        return None

    def cppfuncsig_pointer_for_cfuncsig_pointer(self, cfuncsig_pointer):
        for mapping in self.all_mappings():
            for entry in mapping.funcsig_pointer_mappings:
                if entry.cfuncsig.name == cfuncsig_pointer.name:
                    return entry.cppfuncsig_pointer
        return None
    
    # Searches both C and C++ member and virtual member maps
    def is_member_function(self, function_name):
        for mapping in self.all_mappings():
            for entry in mapping.member_funcsig_mappings:
                if entry.cfuncsig.name == function_name or entry.cppfuncsig.name == function_name:
                    return True
            for entry in mapping.virtual_member_funcsig_mappings:
                if entry.cfuncsig.name == function_name or entry.cppfuncsig.name == function_name:
                    return True
                
        return False

    # ============================== Functions to query the mappings:  end   ==============================
