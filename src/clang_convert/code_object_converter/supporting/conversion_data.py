
import utils.debug as debug
import cpp_code.code_info as code_info
import code_object_converter.supporting.code_mappings as code_mappings
import code_object_converter.supporting.funcsig_mapping as funcsig_mapping
import code_object_converter.supporting.funcsig_pointer_mapping as funcsig_pointer_mapping
from code_object.arg import Arg
from code_object.declaration_specifier import DeclSpec
from code_object_converter.supporting.conversion_data_helper import *

# Store C to C++ conversion data to be used by the converters
#
# The converters will add/update the entries here as required
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
        self._local_mappings = code_mappings.CodeMappings()


    # The info object can be manipulated directly
    @property
    def info(self):
        return self._info

    # ============================== Functions to update the mappings: start ==============================

    @property
    def active_map(self):
        if self._state is ConversionDataState.LOCAL:
            return self._local_mappings
        else:
            return self._global_mappings

    def add_funcbody_type_mapping(self, cdecl_spec, cppdecl_spec):
        assert isinstance(cdecl_spec, DeclSpec), f"Expected DeclSpec, got [{cdecl_spec}]"
        assert isinstance(cppdecl_spec, DeclSpec) or cppdecl_spec==DeclSpec.NONE, f"Expected DeclSpec, got [{cppdecl_spec}]"
        if cdecl_spec in self.active_map.funcbody_type_mappings:
            assert self.active_map.funcbody_type_mappings[cdecl_spec] == cppdecl_spec, f"Updating an existing arg: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}] Previous arg=[{debug.as_debug_string(self.active_map.funcbody_type_mappings[cdecl_spec])}]"
        else:
            self.active_map.funcbody_type_mappings[cdecl_spec] = cppdecl_spec
            debug.line("add_funcbody_type_mapping", f"Adding decl_spec: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}]")

    def add_funcsig_type_mapping(self, cdecl_spec, cppdecl_spec):
        assert isinstance(cdecl_spec, DeclSpec), f"Expected DeclSpec, got [{cdecl_spec}]"
        assert isinstance(cppdecl_spec, DeclSpec) or cppdecl_spec==DeclSpec.NONE, f"Expected DeclSpec, got [{cppdecl_spec}]"
        if cdecl_spec in self.active_map.funcsig_type_mappings:
            assert self.active_map.funcsig_type_mappings[cdecl_spec] == cppdecl_spec, f"Updating an existing arg: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}] Previous arg=[{debug.as_debug_string(self.active_map.funcbody_type_mappings[cdecl_spec])}]"
        else:
            self.active_map.funcsig_type_mappings[cdecl_spec] = cppdecl_spec
            debug.line("add_funcsig_type_mapping", f"Adding decl_spec: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}]")

    def add_funcbody_arg_mapping(self, carg, cpparg):
        assert isinstance(carg, Arg), f"Expected Arg, got [{carg}]"
        assert isinstance(cpparg, Arg) or cpparg==Arg.NONE, f"Expected Arg, got [{cpparg}]"
        if not carg.name:
            debug.line("add_arg_mapping", f"carg name is empty, not adding mapping!")
            return

        if carg in self.active_map.funcbody_arg_mappings:
            assert self.active_map.funcbody_arg_mappings[carg] == cpparg, f"Updating an existing arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}] Previous arg=[{debug.as_debug_string(self.active_map.funcbody_arg_mappings[carg])}]"
        else:
            self.active_map.funcbody_arg_mappings[carg] = cpparg
            debug.line("add_arg_mapping", f"Adding arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}]")

    def add_funcsig_arg_mapping(self, carg, cpparg):
        assert isinstance(carg, Arg), f"Expected Arg, got [{carg}]"
        assert isinstance(cpparg, Arg) or cpparg==Arg.NONE, f"Expected Arg, got [{cpparg}]"
        if carg in self.active_map.funcsig_arg_mappings:
            assert self.active_map.funcsig_arg_mappings[carg] == cpparg, f"Updating an existing funcsig arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}] Previous function arg=[{debug.as_debug_string(self.active_map.function_arg_mappings[carg])}]"
        else:
            self.active_map.funcsig_arg_mappings[carg] = cpparg
            debug.line("add_funcsig_arg_mapping", f"Adding funcsig arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}]")

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

    # NOTE: Local mappings are returned first so that they take precedence!
    def all_mappings(self):
        if self._local_mappings:
            return [self._local_mappings, self._global_mappings]
        else:
            return [self._global_mappings]

    # Find the cppdecl_spec which most closely matches the supplied value
    #
    # Returns a value (or None) and a MatchType.
    # See conversion_data_helper.find_best_matching_cdecl_spec() for more info
    #
    def closest_funcbody_cppdecl_spec_for_cdecl_spec(self, cdecl_spec):
        matches = []
        for mapping in self.all_mappings():
            for key, value in mapping.funcbody_type_mappings.items():
                debug.line("closest_funcbody_cppdecl_spec_for_cdecl_spec", f" Entry: key=[{debug.as_debug_string(key)}] value=[{debug.as_debug_string(value)}]")
                if key == cdecl_spec:
                    return value, DeclSpecMatchType.FULL
                if key.type == cdecl_spec.type:
                    matches.append( (key, value) )

        return create_best_matching_cdecl_spec(cdecl_spec, matches)

    # Funcsig equivalent of closest_cppdecl_spec_for_cdecl_spec
    def closest_funcsig_cppdecl_spec_for_cdecl_spec(self, cdecl_spec):
        matches = []
        for mapping in self.all_mappings():
            for key, value in mapping.funcsig_type_mappings.items():
                debug.line("closest_funcsig_cppdecl_spec_for_cdecl_spec", f" Entry: key=[{debug.as_debug_string(key)}] value=[{debug.as_debug_string(value)}]")
                if key == cdecl_spec:
                    return value, DeclSpecMatchType.FULL
                if key.type == cdecl_spec.type:
                    matches.append( (key, value) )

        return create_best_matching_cdecl_spec(cdecl_spec, matches)

    # Use this version if you only want to match type (and optionally pointer)
    def closest_funcbody_cppdecl_spec_for_ctype(self, type, pointer=None):
        cdecl_spec = DeclSpec(type=type, pointer=pointer)
        return self.closest_funcbody_cppdecl_spec_for_cdecl_spec(cdecl_spec)

    def funcbody_cpparg_for_carg(self, carg):
        for mapping in self.all_mappings():
            for key, value in mapping.funcbody_arg_mappings.items():
                if key.name == carg.name:
                    return value
        return None

    def funcbody_cpparg_for_carg_name(self, carg_name):
        assert carg_name, f"carg_name can't be empty!"

        for mapping in self.all_mappings():
            for key, value in mapping.funcbody_arg_mappings.items():
                if key.name == carg_name:
                    return value
        return None

    def funcsig_cpparg_for_carg(self, carg):
        if not carg.name:
            debug.line("funcsig_cpparg_for_carg", f"carg has no name - can't look up a match")
            return None

        for mapping in self.all_mappings():
            for key, value in mapping.funcsig_arg_mappings.items():
                debug.line("funcsig_cpparg_for_carg", f"key=[{debug.as_debug_string(key)}] carg=[{debug.as_debug_string(carg)}]")
                if key.name == carg.name:
                    return value
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
