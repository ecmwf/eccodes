
import utils.debug as debug
import cpp_code.code_info as code_info
import code_object_converter.conversion_pack.code_mappings as code_mappings
import code_object_converter.conversion_pack.funcsig_mapping as funcsig_mapping
import code_object_converter.conversion_pack.funcsig_pointer_mapping as funcsig_pointer_mapping
from code_object.arg import Arg
from code_object.function_call import FunctionCall
from code_object.funcsig import FuncSig
from code_object.data_member import DataMember
from code_object.declaration_specifier import DeclSpec
from code_object_converter.conversion_pack.conversion_data_helper import *
from code_object.code_interface import NONE_VALUE
import code_object_converter.conversion_pack.buffer_mapping as buffer_mapping
import code_object_converter.conversion_pack.arg_utils as arg_utils
from copy import deepcopy

# Store C to C++ conversion data to be used by the converters
#
# The converters will add/update the entries here as required
class ConversionData:
    def __init__(self, info) -> None:
        self._state = ConversionDataState.GLOBAL
        self._info = info
        self._global_mappings = code_mappings.CodeMappings()
        self._local_mappings = None

        # The name of the c function currently being processed
        self._current_cfuncname = ""

    # Call this to ensure local state is set ready for function conversions
    def set_local_state(self):
        self._state = ConversionDataState.LOCAL
        self.reset_local_state("")

    # Clear out / reset local state data ready to convert a new function (cfuncname)
    def reset_local_state(self, cfuncname):
        self._local_mappings = code_mappings.CodeMappings()
        self._current_cfuncname = cfuncname
        debug.line("reset_local_state", f"cfuncname=[{cfuncname}]")

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
            self.active_map.funcbody_type_mappings[deepcopy(cdecl_spec)] = deepcopy(cppdecl_spec)
            debug.line("add_funcbody_type_mapping", f"Adding decl_spec: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}]")

    def add_funcsig_type_mapping(self, cdecl_spec, cppdecl_spec):
        assert isinstance(cdecl_spec, DeclSpec), f"Expected DeclSpec, got [{cdecl_spec}]"
        assert isinstance(cppdecl_spec, DeclSpec) or cppdecl_spec==DeclSpec.NONE, f"Expected DeclSpec, got [{cppdecl_spec}]"
        if cdecl_spec in self.active_map.funcsig_type_mappings:
            assert self.active_map.funcsig_type_mappings[cdecl_spec] == cppdecl_spec, f"Updating an existing arg: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}] Previous arg=[{debug.as_debug_string(self.active_map.funcbody_type_mappings[cdecl_spec])}]"
        else:
            self.active_map.funcsig_type_mappings[deepcopy(cdecl_spec)] = deepcopy(cppdecl_spec)
            debug.line("add_funcsig_type_mapping", f"Adding decl_spec: [{debug.as_debug_string(cdecl_spec)}] -> [{debug.as_debug_string(cppdecl_spec)}]")

    def add_funcsig_buffer_mapping(self, cbuffer, clength, cpp_container):
        mapping = buffer_mapping.BufferMapping(cbuffer=cbuffer, clength=clength, cpp_container=cpp_container)
        debug.line("add_funcsig_buffer_mapping", f"Adding [{mapping.as_string()}]")
        self.active_map.funcsig_buffer_mappings.append(mapping)

    def add_funcbody_arg_mapping(self, carg, cpparg):
        assert isinstance(carg, Arg), f"Expected Arg, got [{carg}]"
        assert isinstance(cpparg, Arg) or cpparg==NONE_VALUE, f"Expected Arg, got [{cpparg}]"
        if not carg.name:
            debug.line("add_arg_mapping", f"carg name is empty, not adding mapping!")
            return

        if carg in self.active_map.funcbody_arg_mappings:
            assert self.active_map.funcbody_arg_mappings[carg] == cpparg, f"Updating an existing arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}] Previous arg=[{debug.as_debug_string(self.active_map.funcbody_arg_mappings[carg])}]"
        else:
            self.active_map.funcbody_arg_mappings[carg] = cpparg
            debug.line("add_arg_mapping", f"Adding arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}]")

    def add_data_member_mapping(self, cmember, cppmember):
        assert isinstance(cmember, DataMember), f"Expected DataMember, got [{cmember}]"
        assert isinstance(cppmember, DataMember) or cppmember==NONE_VALUE, f"Expected DataMember, got [{cppmember}]"
        if not cmember.name:
            debug.line("add_data_member_mapping", f"cmember name is empty, not adding mapping!")
            return

        if cmember in self.active_map.data_member_mappings:
            assert self.active_map.data_member_mappings[deepcopy(cmember)] == deepcopy(cppmember), f"Updating an existing data member: [{debug.as_debug_string(cmember)}] -> [{debug.as_debug_string(cppmember)}] Previous value=[{debug.as_debug_string(self.active_map.data_member_mappings[cmember])}]"
        else:
            self.active_map.data_member_mappings[cmember] = cppmember
            debug.line("add_data_member_mapping", f"Adding data member: [{debug.as_debug_string(cmember)}] -> [{debug.as_debug_string(cppmember)}]")

    def add_funcsig_arg_mapping(self, carg, cpparg):
        assert isinstance(carg, Arg), f"Expected Arg, got [{carg}]"
        assert isinstance(cpparg, Arg) or cpparg==NONE_VALUE, f"Expected Arg, got [{cpparg}]"
        if carg in self.active_map.funcsig_arg_mappings:
            assert self.active_map.funcsig_arg_mappings[carg] == cpparg, f"Updating an existing funcsig arg: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpparg)}] Previous function arg=[{debug.as_debug_string(self.active_map.funcsig_arg_mappings[carg])}]"
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

    # These need to be global as all functions may access them...
    def add_global_member_funcsig_mapping(self, mapping):
        debug.line("add_global_member_funcsig_mapping", f">>>>> [IN:SELF]=[{self}]")
        debug.line("add_global_member_funcsig_mapping", f">>>>> [IN] cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")
        assert isinstance(mapping, funcsig_mapping.FuncSigMapping), f"Expected FuncSigMapping, got type=[{type(mapping).__name__}]"
        for entry in self._global_mappings.member_funcsig_mappings:

            debug.line("add_global_member_funcsig_mapping", f">>>>> entry.cfuncsig.name=[{entry.cfuncsig.name}] mapping.cfuncsig.name=[{mapping.cfuncsig.name}]")

            if entry.cfuncsig.name == mapping.cfuncsig.name and entry.cppfuncsig:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"

        self._global_mappings.member_funcsig_mappings.append(mapping)

        debug.line("add_global_member_funcsig_mapping", f">>>>> [OUT]")

    def add_global_virtual_member_funcsig_mapping(self, mapping):
        assert isinstance(mapping, funcsig_mapping.FuncSigMapping), f"Expected FuncSigMapping, got type=[{type(mapping).__name__}]"
        for entry in self._global_mappings.virtual_member_funcsig_mappings:
            if entry.cfuncsig.name == mapping.cfuncsig.name:
                assert False, f"Mapping for [{mapping.cfuncsig.name}] already exists!"
        
        self._global_mappings.virtual_member_funcsig_mappings.append(mapping)

    def add_literal_mapping(self, cstring, cppstring):
        assert isinstance(cstring, str), f"Expected str, got [{cstring}]"
        assert isinstance(cppstring, str), f"Expected str, got [{cppstring}]"

        if cstring in self.active_map.literal_mappings:
            assert self.active_map.literal_mappings[cstring] == cppstring, f"Updating an existing literal: [{cstring}] -> [{cppstring}] Previous value=[{self.active_map.literal_mappings[cstring]}]"
        else:
            self.active_map.literal_mappings[cstring] = cppstring
            debug.line("add_literal_mapping", f"Adding literal mapping: [{cstring}] -> [{cppstring}]")

    # Names referring to self class
    def add_self_class_pointer_name(self, name):
        assert isinstance(name, str), f"Expected str, got [{name}]"
        if name not in self.active_map.self_class_pointer_names:
            self.active_map.self_class_pointer_names.append(name)

    def add_container_type(self, type):
        assert isinstance(type, str), f"Expected str, got [{type}]"
        if type not in self.active_map.container_types:
            self.active_map.container_types.append(type)

    # Converts the function call into a funcsig object (with void return type) and stores in the info
    # object. This allows all arg type information to be available (for example when post-processing)
    def add_cppfunction_call(self, cppfunction_call):
        assert isinstance(cppfunction_call, FunctionCall), f"Expected FunctionCall, got [{cppfunction_call}]"

        cppargs = []
        for entry in cppfunction_call.args:
            cppname = arg_utils.extract_name(entry)
            if not cppname:
                debug.line("add_cppfunction_call", f"Couldn't deduce name for entry=[{debug.as_debug_string(entry)}] cppfunction_call=[{debug.as_debug_string(cppfunction_call)}]")
                return

            cpparg = self.cpparg_for_cname(cppname)
            debug.line("add_cppfunction_call", f"[{debug.as_debug_string(cppfunction_call)}]---> entry type=[{type(entry)}] value=[{debug.as_debug_string(entry)}] cppname=[{debug.as_debug_string(cppname)}] cpparg=[{debug.as_debug_string(cpparg)}]")

            if cpparg and cpparg != NONE_VALUE:
                cppargs.append(cpparg)

        cppfuncsig = FuncSig("void", cppfunction_call.name, cppargs)

        self._info.add_function_call_entry(cppfuncsig)
        debug.line("add_cppfunction_call", f"Added function call cppfunction_call=[{debug.as_debug_string(cppfunction_call)}] -> cppfuncsig=[{debug.as_debug_string(cppfuncsig)}]")

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
                debug.line("closest_funcbody_cppdecl_spec_for_cdecl_spec", f" Entry: key=[{debug.as_debug_string(key)}] [{key}] value=[{debug.as_debug_string(value)}]")
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
                debug.line("funcbody_cpparg_for_carg", f"key=[{debug.as_debug_string(key)}] value=[{debug.as_debug_string(value)}]")
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
    
    # Use this version when you only have a name and need to get the full arg (inc decl_type)
    def funcbody_cpparg_for_cpparg_name(self, cpparg_name):
        assert cpparg_name, f"cpparg_name can't be empty!"

        for mapping in self.all_mappings():
            for key, value in mapping.funcbody_arg_mappings.items():
                if value != NONE_VALUE and value.name == cpparg_name:
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

    # Use this version when you only have a name and need to get the full arg (inc decl_type)
    def funcsig_cpparg_for_carg_name(self, carg_name):
        assert carg_name, f"carg_name can't be empty!"

        for mapping in self.all_mappings():
            for key, value in mapping.funcsig_arg_mappings.items():
                if key.name == carg_name:
                    return value
        return None

    # Use this version when you only have a name and need to get the full arg (inc decl_type)
    def funcsig_cpparg_for_cpparg_name(self, cpparg_name):
        assert cpparg_name, f"cpparg_name can't be empty!"

        for mapping in self.all_mappings():
            for key, value in mapping.funcsig_arg_mappings.items():
                if value != NONE_VALUE and value.name == cpparg_name:
                    return value
        return None

    # Given the cppname, search all stores to see if an arg exists
    def cpparg_for_cppname(self, cppname):
        cpparg = self.funcbody_cpparg_for_cpparg_name(cppname)
        if cpparg:
            return cpparg
        
        cpparg = self.funcsig_cpparg_for_cpparg_name(cppname)
        if cpparg:
            return cpparg

        buf_map = self.funcsig_buffer_mapping_for_cppname(cppname)
        if buf_map:
            return buf_map.cpp_container

        return None

    # Given the cname, search all stores to see if an arg exists
    def cpparg_for_cname(self, cname):
        cpparg = self.funcbody_cpparg_for_carg_name(cname)
        if cpparg:
            return cpparg
        
        cpparg = self.funcsig_cpparg_for_carg_name(cname)
        if cpparg:
            return cpparg

        buf_map = self.funcsig_buffer_mapping_for_cname(cname)
        if buf_map:
            return buf_map.cpp_container

        return None

    def cppdata_member_for_cdata_member(self, cmember):
        for mapping in self.all_mappings():
            for key, value in mapping.data_member_mappings.items():
                debug.line("cppdata_member_for_cdata_member", f"key=[{debug.as_debug_string(key)}] value=[{debug.as_debug_string(value)}]")
                if key.name == cmember.name:
                    return value
        return None

    def cppdata_member_for_cdata_member_name(self, cmember_name):
        for mapping in self.all_mappings():
            for key, value in mapping.data_member_mappings.items():
                debug.line("cppdata_member_for_cdata_member_name", f"key=[{debug.as_debug_string(key)}] value=[{debug.as_debug_string(value)}]")
                if key.name == cmember_name:
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

    def cppfuncsig_for_cppfuncname(self, cppfuncname):
        for mapping in self.all_mappings():
            for entry in mapping.all_funcsig_mappings:
                if entry.cppfuncsig != NONE_VALUE and entry.cppfuncsig.name == cppfuncname:
                    return entry.cppfuncsig
        return None

    def funcsig_mapping_for_cfuncname(self, cfuncname):
        for mapping in self.all_mappings():
            for entry in mapping.all_funcsig_mappings:
                if entry.cfuncsig.name == cfuncname and entry.cppfuncsig:
                    return entry
        return None
    
    def funcsig_mapping_for_current_cfuncname(self):
        return self.funcsig_mapping_for_cfuncname(self._current_cfuncname)

    def funcsig_buffer_mapping_for_cname(self, cname):
        for mapping in self.all_mappings():
            for entry in mapping.funcsig_buffer_mappings:
                if entry.cbuffer.name == cname or entry.clength.name == cname:
                    return entry
        return None

    def funcsig_buffer_mapping_for_cppname(self, cppname):
        for mapping in self.all_mappings():
            for entry in mapping.funcsig_buffer_mappings:
                if entry.cpp_container.name == cppname:
                    return entry
        return None

    def cppfuncsig_pointer_for_cfuncsig_pointer(self, cfuncsig_pointer):
        for mapping in self.all_mappings():
            for entry in mapping.funcsig_pointer_mappings:
                if entry.cfuncsig.name == cfuncsig_pointer.name:
                    return entry.cppfuncsig_pointer
        return None
    
    def cppstring_literal_for_cstring(self, cstring):
        for mapping in self.all_mappings():
            for key, value in mapping.literal_mappings.items():
                if key == cstring:
                    return value
        return None

    #def funcsig_buffer_mapping_for

    # Searches both C and C++ member and virtual member maps
    # NOTE: cppfuncsig will be none for "discovered" virtual functions until they have been converted!
    def is_member_function(self, function_name):
        
        for entry in self._global_mappings.member_funcsig_mappings:
            if entry.cfuncsig.name == function_name:
                return True
            if entry.cppfuncsig and entry.cppfuncsig != NONE_VALUE and entry.cppfuncsig.name == function_name:
                return True
            
        for entry in self._global_mappings.virtual_member_funcsig_mappings:
            if entry.cfuncsig.name == function_name:
                return True
            if entry.cppfuncsig and entry.cppfuncsig != NONE_VALUE and entry.cppfuncsig.name == function_name:
                return True
                
        return False

    def is_self_class_pointer_name(self, name):
        debug.line("is_self_class_pointer_name", f"Testing name=[{debug.as_debug_string(name)}]")
        for mapping in self.all_mappings():
            for entry in mapping.self_class_pointer_names:
                debug.line("is_self_class_pointer_name", f" > Testing entry=[{debug.as_debug_string(entry)}]")
                if entry == name:
                    debug.line("is_self_class_pointer_name", f" RESULT: TRUE")
                    return True

        debug.line("is_self_class_pointer_name", f" RESULT: FALSE")
        return False

    def is_container_type(self, type):
        for mapping in self.all_mappings():
            for entry in mapping.container_types:
                # NOTE: type may be a reference, std::string& versus std::string, but we still want to match...
                if entry in type:
                    return True

        return False

    # ============================== Functions to query the mappings:  end   ==============================
