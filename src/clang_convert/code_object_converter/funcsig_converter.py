
import debug
import code_object.funcsig as funcsig
import code_object_converter.arg_converter as arg_converter
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.funcsig_mapping as funcsig_mapping
from standard_transforms import transform_function_name

# Convert a C Function Signature to C++

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
class FuncSigConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, funcsig.FuncSig), f"Expected FuncSig, got type=[{type(ccode_object)}]"

        self._conversions = []

    # Returns both the new cpp funcsig and an updated c funcsig
    def to_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data

        # If we have a mapping already stored, just return that!
        cppfuncsig = self._conversion_data.cppfuncsig_for_cfuncsig(self._ccode_object)
        if cppfuncsig:
            return cppfuncsig

        cppfuncsig = funcsig.FuncSig(
            self.to_cpp_return_type(), 
            self.to_cpp_name(), 
            self.to_cpp_args(),
            self._ccode_object.template)

        #cppfuncsig.static = self.is_cpp_static()

        # Add this to the conversion data mappings
        mapping = funcsig_mapping.FuncSigMapping(self._ccode_object, cppfuncsig)
        self._conversion_data.add_to_funcsig_mappings(mapping)

        return cppfuncsig

    # Override to customise the static behaviour
    def is_cpp_static(self):
        return self._ccode_object.static

    def to_cpp_return_type(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._ccode_object.name:
                return entry.cppfuncsig.return_type

        if self._ccode_object.return_type == "int":
            # We'll assume int means GribStatus
            return "GribStatus"
        else:
            return self._ccode_object.return_type

    def to_cpp_name(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._ccode_object.name:
                return entry.cppfuncsig.name
        
        return transform_function_name(self._ccode_object.name)

    # This should return the same number of cppargs as there are cargs (set unused cppargs to None)
    def to_cpp_args(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._ccode_object.name:
                return entry.cppfuncsig.args

        cppargs = []
        for entry in self._ccode_object.args:
            arg_conv = arg_converter.ArgConverter(entry)
            cpparg = arg_conv.to_cpp_code_object(self._conversion_data)
            cppargs.append(cpparg)

        return cppargs
    
    # Get the buffer mappings from C {ptr,len} to C++ container, or None if there aren't any
    def arg_indexes(self):
        for entry in self._conversions:
            if entry.cfuncsig.name == self._ccode_object.name:
                return entry.arg_indexes
        
        return None
  