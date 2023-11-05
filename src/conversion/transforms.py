# C to C++ transforms, including arg, type, function name

import re
import debug
import arg
import copy
from funcsig_mapping import FuncSigMapping

# Transforms are:
#
# types                      : {ctype, cpptype}   	  type = "int", "GribStatus" etc
# args                       : {carg,  cpparg}    	  arg = arg.Arg(type, name)
# members                    : {cmember,  cppmember}  member = member.Member(type, name)
# inherited_funcsig_mappings : [funcsig_mapping, ...]
# private_funcsig_mappings   : [funcsig_mapping, ...]
# static_funcsig_mappings    : [funcsig_mapping, ...]

class Transforms:
    def __init__(self, *, funcsig_types={}, types={}) -> None:
        self._funcsig_types = funcsig_types
        self._types = types
        self._function_pointers = {}
        self._all_args = {}
        self._global_args = {}
        self._members = {}
        self._inherited_funcsig_mappings = []
        self._private_funcsig_mappings = []
        self._static_funcsig_mappings = []
        self._other_funcsig_mappings = []

    # Note these are only supplied at __init__, any new types are added to the types list instead
    @property
    def funcsig_types(self):
        return self._funcsig_types

    @property
    def types(self):
        return self._types

    def add_to_types(self, ctype, cpptype):
        if ctype in self._types:
            assert self._types[ctype] == cpptype, f"Updating an existing type transform: {ctype} -> {cpptype} Previous type = {self._types[ctype]}"
        else:
            self._types[ctype] = cpptype
            debug.line("Transforms", f"Adding type transform: {ctype} -> {cpptype}")

    # Store C -> C++ Function Pointer names for reference
    @property
    def function_pointers(self):
        return self._function_pointers

    def add_to_function_pointers(self, cfuncname, cppfuncname):
        if cfuncname in self._function_pointers:
            assert self._function_pointers[cfuncname] == cppfuncname, f"Updating an existing function pointer: {cfuncname} -> {cppfuncname} Previous type = {self._function_pointers[cfuncname]}"
        else:
            self._function_pointers[cfuncname] = cppfuncname
            debug.line("Transforms", f"Adding function pointer transform: {cfuncname} -> {cppfuncname}")
            # We'll add to the types list too
            self.add_to_types(cfuncname, cppfuncname)

    @property
    def all_args(self):
        return self._all_args
    
    # Helper to return the ctype for the supplied cname, or None
    def ctype_of(self, cname):
        for carg in self.all_args.keys():
            if carg.name == cname:
                return carg.type

        return None

    # Helper to return the cpparg for the supplied cname, or None
    def cpparg_for(self, cname):
        for carg, cpparg in self.all_args.items():
            if carg.name == cname:
                return cpparg

        return None

    # Helper to return the cpptype for the supplied cppname, or None
    def cpptype_of(self, cppname):
        for cpparg in self.all_args.values():
            if cpparg and cpparg.name == cppname:
                return cpparg.type

        return None

    def add_local_args(self, carg, cpparg):
        if carg in self._all_args:
            assert self._all_args[carg] == cpparg, f"Updating an existing local arg transform: C Arg = {arg.arg_string(carg)} -> {arg.arg_string(cpparg)} Previous arg = {arg.arg_string(self._all_args[carg])}"
        else:
            debug.line("Transforms", f"Adding new local arg transform: {arg.arg_string(carg)} -> {arg.arg_string(cpparg)}")
            assert carg, f"ADDING carg which is None!"
            self._all_args[carg] = cpparg
    
    def clear_local_args(self):
        self._all_args.clear()
        self._all_args = copy.copy(self._global_args)

    @property
    def global_args(self):
        return self._global_args

    # Convert to a set of global transforms by moving the local transforms to the global area
    # and deleting the local transforms.
    # Call this after processing the global function to set up for all other function conversions
    def make_global(self):
        assert not self._global_args, "unexpected make_global call when global_args not empty"
        self._global_args = copy.deepcopy(self._all_args)
        self.clear_local_args()

    @property
    def members(self):
        return self._members

    def add_to_members(self, cmember, cppmember):
        if cmember in self._members:
            assert self._members[cmember] == cppmember, f"Updating an existing member transform: C member = {arg.arg_string(cmember)} -> {arg.arg_string(cppmember)} Previous member = {arg.arg_string(self._members[cmember])}"
        else:
            self._members[cmember] = cppmember

    # Helper to get the C++ funcsig that matches the supplied C function name
    def cppfuncsig_for(self, cfuncname):
        mapping = self.funcsig_mapping_for(cfuncname)

        if mapping:
            return mapping.cppfuncsig
        else:
            return None

    # Helper to search all mappings and return the one that matches the supplied C function name, or None
    def funcsig_mapping_for(self, cfuncname):
        for entry in self.inherited_funcsig_mappings:
            if cfuncname == entry.cfuncsig.name:
                return entry
        for entry in self.private_funcsig_mappings:
            if cfuncname == entry.cfuncsig.name:
                return entry
        for entry in self.static_funcsig_mappings:
            if cfuncname == entry.cfuncsig.name:
                return entry
        for entry in self.other_funcsig_mappings:
            if cfuncname == entry.cfuncsig.name:
                return entry

        return None

    @property
    def inherited_funcsig_mappings(self):
        return self._inherited_funcsig_mappings

    def add_to_inherited_funcsig_mappings(self, mapping):
        for entry in self._inherited_funcsig_mappings:
            assert entry.cfuncsig.name != mapping.cfuncsig.name, f"Setting an existing inherited_funcsig_mappings transform: {mapping.cfuncsig.name} -> {mapping.cppfuncsig.name}"
        self._inherited_funcsig_mappings.append(mapping)

    @property
    def private_funcsig_mappings(self):
        return self._private_funcsig_mappings

    def add_to_private_funcsig_mappings(self, mapping):
        for entry in self._private_funcsig_mappings:
            assert entry.cfuncsig.name != mapping.cfuncsig.name, f"Setting an existing private_funcsig_mappings transform: {mapping.cfuncsig.name} -> {mapping.cppfuncsig.name}"
        self._private_funcsig_mappings.append(mapping)

    @property
    def static_funcsig_mappings(self):
        return self._static_funcsig_mappings

    def add_to_static_funcsig_mappings(self, mapping):
        for entry in self._static_funcsig_mappings:
            assert entry.cfuncsig.name != mapping.cfuncsig.name, f"Setting an existing static_funcsig_mappings transform: {mapping.cfuncsig.name} -> {mapping.cppfuncsig.name}"
        self._static_funcsig_mappings.append(mapping)

    # Other funcsigs are for Global, Constructor and Destructor
    # Making them available for reference via funcsig_for()
    @property
    def other_funcsig_mappings(self):
        return self._other_funcsig_mappings

    def add_to_other_funcsig_mappings(self, mapping):
        for entry in self._other_funcsig_mappings:
            assert entry.cfuncsig.name != mapping.cfuncsig.name, f"Setting an existing other_funcsig_mappings transform: {mapping.cfuncsig.name} -> {mapping.cppfuncsig.name}"
        self._other_funcsig_mappings.append(mapping)
