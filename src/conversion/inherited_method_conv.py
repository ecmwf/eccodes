
from method_conv import *
import inherited_method
import inherited_method_funcsig_conv

# Specialisation of MethodConverter for AccessorData Methods
class InheritedMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return inherited_method.InheritedMethod(cppfuncsig, self._transforms.types["self"])

    # Comment out inherited methods that we aren't ready to implement yet...
    def create_commented_cpp_body(self):
        debug.line("create_commented_cpp_body", f"\n============================== {self._cfunction.name} [IN]  ==============================\n")
        
        cpp_lines = []
        cpp_lines.append("// C++ implementation not yet available.")
        cpp_lines.append("// Commented C body provided below for reference:\n")
        
        for line in self._cfunction.code:
            cpp_lines.append("// " + line)
        cpp_lines.append("")
        
        cpp_lines.append(f"return {self._transforms.types['super']}::{self._cppfunction.name}({self._cppfunction.parent_call_arg_string});")
        
        debug.line("create_commented_cpp_body", f"\n============================== {self._cfunction.name} [OUT] ==============================\n")

        return cpp_lines


    # Overridden to handle specific cases
    def create_cpp_body(self):
        if self._cppfunction.name in ["dump", "compare"]:
            return self.create_commented_cpp_body()

        return super().create_cpp_body()

    # Version of transform_cfunction_call for inherited methods
    # Added to avoid traversing the map twice through calls to convert_cfunction_to_cpp_format
    # Calls directly to transform_cfunction_call will also come through here...
    def transform_cinherited_method_call(self, cfuncname, cparams):

        m = re.match(r"grib_(\w*)", cfuncname)
        if m:
            cmethod_name = m.group(1)
        else:
            cmethod_name = cfuncname

        # Need to use the full list in case the current method's local mappings don't support it!
        conversions = inherited_method_funcsig_conv.InheritedMethodFuncSigConverter.inherited_method_conversions

        cppfuncname, transformed_cparams = self.transform_cfunction_call_from_conversions(cmethod_name, cparams, conversions)

        return cppfuncname, transformed_cparams

    # Overridden to handle inherited methods, and also grib_pack_long() etc that
    # convert to accessor_ptr->packLong()
    def transform_cfunction_call(self, cfuncname, cparams):

        cppfuncname, transformed_cparams = self.transform_cinherited_method_call(cfuncname, cparams)

        if cppfuncname:
            return cppfuncname, transformed_cparams

        return super().transform_cfunction_call(cfuncname, cparams)

    # Overridden e.g. to convert foo(a,b) to a->foo(b) (as required)
    def convert_cfunction_to_cpp_format(self, cfuncname, cparams):
        cppfuncname, transformed_cparams = self.transform_cinherited_method_call(cfuncname, cparams)

        if cppfuncname:
            assert len(cparams) >= 1, f"Expected at least 1 parameter for function [{cfuncname}]"
            return f"{cparams[0]}->{cppfuncname}({','.join([p for p in transformed_cparams ])})"

        return super().convert_cfunction_to_cpp_format(cfuncname, cparams)

