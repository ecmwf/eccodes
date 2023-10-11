# C to C++ transforms, including arg, type, function name

import re
import debug
import arg
from collections import namedtuple
import copy

# Transforms are:
#
# types              : {ctype, cpptype}   	  type = "int", "GribStatus" etc
# args               : {carg,  cpparg}    	  arg = arg.Arg(type, name)
# members            : {cmember,  cppmember}  member = member.Member(type, name)
# inherited_funcsigs : {cfuncsig, cppfuncsig} funcsig = func_sig.FuncSig(...)
# private_funcsigs   : {cfuncsig, cppfuncsig} funcsig = func_sig.FuncSig(...)
# static_funcsigs    : {cfuncsig, cppfuncsig} funcsig = func_sig.FuncSig(...)
# class_types        : {"Entry", {cvalue, cppvalue} }   "Entry" is "self", "super", etc

# These entries are expected to exist!
default_class_type_entries = ["self", "super"]

# Allow access of class_types tuples using .cvalue and .cppvalue for convenience!
TypePair = namedtuple('TypePair', ['cvalue', 'cppvalue'])

class Transforms:
    def __init__(self, *, types={}, members={}, inherited_funcsigs={}, private_funcsigs={}, static_funcsigs={}) -> None:
        self._types = types
        self._all_args = {}
        self._global_args = {}
        self._members = members
        self._inherited_funcsigs = inherited_funcsigs
        self._private_funcsigs = private_funcsigs
        self._static_funcsigs = static_funcsigs
        self._class_types = {}

    @property
    def types(self):
        return self._types

    def add_to_types(self, ctype, cpptype):
        if ctype in self._types:
            assert self._types[ctype] == cpptype, f"Updating an existing type transform: {ctype} -> {cpptype} Previous type = {self._types[ctype]}"
        else:
            self._types[ctype] = cpptype
            debug.line("Transforms", f"Adding type transform: {ctype} -> {cpptype}")

    @property
    def all_args(self):
        return self._all_args

    def add_local_args(self, carg, cpparg):
        if carg in self._all_args:
            assert self._all_args[carg] == cpparg, f"Updating an existing local arg transform: C Arg = {arg.arg_string(carg)} -> {arg.arg_string(cpparg)} Previous arg = {arg.arg_string(self._all_args[carg])}"
        else:
            debug.line("Transforms", f"Adding new local arg transform: {arg.arg_string(carg)} -> {arg.arg_string(cpparg)}")
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

    @property
    def inherited_funcsigs(self):
        return self._inherited_funcsigs

    def add_to_inherited_funcsigs(self, cfuncsig, cppfuncsig):
        assert cfuncsig not in self._inherited_funcsigs, f"Setting an existing inherited_funcsig transform: {cfuncsig.name} -> {cppfuncsig.name}"
        self._inherited_funcsigs[cfuncsig] = cppfuncsig

    @property
    def private_funcsigs(self):
        return self._private_funcsigs

    def add_to_private_funcsigs(self, cfuncsig, cppfuncsig):
        assert cfuncsig not in self._private_funcsigs, f"Setting an existing private_funcsig transform: {cfuncsig.name} -> {cppfuncsig.name}"
        self._private_funcsigs[cfuncsig] = cppfuncsig

    @property
    def static_funcsigs(self):
        return self._static_funcsigs

    def add_to_static_funcsigs(self, cfuncsig, cppfuncsig):
        assert cfuncsig not in self._static_funcsigs, f"Setting an existing static_funcsig transform: {cfuncsig.name} -> {cppfuncsig.name}"
        self._static_funcsigs[cfuncsig] = cppfuncsig

    @property
    def class_types(self):
        return self._class_types

    def add_to_class_types(self, entry, cvalue, cppvalue):
        assert entry not in self._class_types, f"Updating an existing class type entry[{entry}] with cvalue={cvalue} cppvalue={cppvalue}"

        self._class_types[entry] = TypePair(cvalue, cppvalue)
