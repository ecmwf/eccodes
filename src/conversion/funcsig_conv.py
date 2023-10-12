# Convert C function signature to C++
import debug
import funcsig
import arg_conv
import re

# Change C-style snake_case function name to C++-style camelCase name
# Also, remove get_ and set_ prefixes
def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return arg_conv.transform_variable_name(name)

# Generic function signature converter - provides basic C to C++ conversion
# Sub-classes should populate the self._conversions dict with specialised conversions
#
# The conversions dictionary defines well-known conversions from C function name to the equivalent 
# C++ function signature, for example:
# sample_conversions = {
#    "pack_string" : FuncSig("GribStatus", "pack", [None, Arg("std::string const&", "value"), None]),
# }
#
# [Key]     Name of an inherited function as defined in the C code (func_name in the functions below)
# [Value]   Signature for the equivalent function in C++
#           The argument list provides a 1:1 mapping from the C equivalent function - this map is also used 
#           when replacing the use of these in the function body
#           Note: some C args don't have a C++ equivalent, so are listed as None to maintain correct mapping
class FuncSigConverter:
    def __init__(self, cfuncsig):
        self._cfuncsig = cfuncsig
        self._conversions = {}

    # Returns both the new cpp funcsig and an updated c funcsig (with any metadata added)
    def to_cpp_funcsig(self):
        cppfuncsig = funcsig.FuncSig(
            self.to_cpp_return_type(), 
            self.to_cpp_name(), 
            self.to_cpp_args(),
            self._cfuncsig.template)
        
        cppfuncsig.static = self._cfuncsig.static
        cfuncsig = self._cfuncsig

        for k,v in self.cmetadata.items():
            if k:
                cfuncsig.add_metadata(k, v)

        return cfuncsig, cppfuncsig

    def to_cpp_return_type(self):
        if self._cfuncsig.name in self._conversions:
            return self._conversions[self._cfuncsig.name].return_type
        elif self._cfuncsig.return_type == "int":
            # We'll assume int means GribStatus
            return "GribStatus"
        else:
            return self._cfuncsig.return_type

    def to_cpp_name(self):
        if self._cfuncsig.name in self._conversions:
            return self._conversions[self._cfuncsig.name].name
        else:
            return transform_function_name(self._cfuncsig.name)

    # This should return the same number of cppargs as there are cargs (set unused cppargs to None)
    def to_cpp_args(self):
        if self._cfuncsig.name in self._conversions:
            return self._conversions[self._cfuncsig.name].args
        else:
            cppargs = []
            for entry in self._cfuncsig.args:
                arg_converter = arg_conv.ArgConverter(entry)
                cpparg = arg_converter.to_cpp_func_sig_arg()
                cppargs.append(cpparg)

            return cppargs
    
    # Override to return any relevant metadata
    @property
    def cmetadata(self):
        return {}
