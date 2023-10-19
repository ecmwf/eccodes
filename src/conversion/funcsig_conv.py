# Convert C function signature to C++
import debug
import funcsig
import arg_conv
import re
from funcsig_mapping import FuncSigMapping

# Change C-style snake_case function name to C++-style camelCase name
# Also, remove get_ and set_ prefixes
def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return arg_conv.transform_variable_name(name)

# Generic function signature converter - provides basic C to C++ conversion
# Sub-classes should populate the self._conversions list with specialised conversions
#
# The conversions list defines well-known conversions from C function name to the equivalent 
# C++ function signature, and identifies arg indexes mapping C buffers {ptr,len} to C++ containers, for example:
# sample_conversions = [
#    FuncSigMapping(FuncSig("int", "pack_string", [Arg("grib_accessor*", "a"), Arg("const char*", "v"), Arg("size_t*", "len")]),
#                FuncSig("GribStatus", "pack", [None, Arg("std::string const&", "value"),  None]),
#                ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
#
# FuncSig           Signature for the C and C++ functions
#                   The argument lists provide a 1:1 mapping from the C to C++ equivalent function - this map is also used 
#                   when replacing the use of these in the function body
#                   Note 1: some C args don't have a C++ equivalent, so are listed as None to maintain correct mapping
#                   Note 2: If there is no C++ equivalent function, then FuncSig(None, None, [None]) will be returned 
#                       (where the number of args in the third parameter will match the C FuncSig)
# ArgIndexes        Provides the index for each arg in the buffer mappings from C {ptr,len} to C++ container
#
class FuncSigConverter:
    def __init__(self, cfuncsig):
        self._cfuncsig = cfuncsig
        self._conversions = []
        self._transforms = None  # Pass in as required

    def create_funcsig_mapping(self, transforms):
        self._transforms = transforms
        cfuncsig, cppfuncsig = self.to_cpp_funcsig()
        mapping = FuncSigMapping(cfuncsig, cppfuncsig, self.arg_indexes())
        return mapping

    # Returns both the new cpp funcsig and an updated c funcsig
    def to_cpp_funcsig(self):
        cppfuncsig = funcsig.FuncSig(
            self.to_cpp_return_type(), 
            self.to_cpp_name(), 
            self.to_cpp_args(),
            self._cfuncsig.template)

        cppfuncsig.static = self._cfuncsig.static
        cfuncsig = self._cfuncsig

        return cfuncsig, cppfuncsig

    def to_cpp_return_type(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._cfuncsig.name:
                return entry.cppfuncsig.return_type

        if self._cfuncsig.return_type == "int":
            # We'll assume int means GribStatus
            return "GribStatus"
        else:
            return self._cfuncsig.return_type

    def to_cpp_name(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._cfuncsig.name:
                return entry.cppfuncsig.name
        
        return transform_function_name(self._cfuncsig.name)

    # This should return the same number of cppargs as there are cargs (set unused cppargs to None)
    def to_cpp_args(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._cfuncsig.name:
                return entry.cppfuncsig.args

        cppargs = []
        for entry in self._cfuncsig.args:
            arg_converter = arg_conv.ArgConverter(entry)
            cpparg = arg_converter.to_cpp_func_sig_arg(self._transforms)
            cppargs.append(cpparg)

        return cppargs
    
    # Get the buffer mappings from C {ptr,len} to C++ container, or None if there aren't any
    def arg_indexes(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._cfuncsig.name:
                return entry.arg_indexes
        
        return None
  
