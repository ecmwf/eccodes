
from func_conv import *
import global_func

# Specialisation of FunctionConverter for global functions
# static_func_name_transforms is a list of converted C -> C++ static function names
# that can be used for substitutions before these functions have actually been parsed
# There are situations where this is required (as described in grib_accessor_conv.py)
class GlobalFunctionConverter(FunctionConverter):
    def __init__(self, static_func_name_transforms):
        self._static_func_name_transforms = static_func_name_transforms
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return global_func.GlobalFunction(cppfuncsig)

    # Overridden to apply any static_func_name_transforms
    def transform_cfunction_name(self, cfunction_name):
        for cfuncname, cppfuncname in self._static_func_name_transforms.items():
            if cfunction_name == cfuncname:
                return cppfuncname

        return super().transform_cfunction_name(cfunction_name)

    # Overridden to detect @FORWARD_DECLARATION placeholder...
    def skip_line(self, line):
        # Ignore @FORWARD_DECLARATION
        if line and line.startswith("@FORWARD_DECLARATION:"):
            debug.line("skip_line", f"[Forward declaration]: {line}")
            return True
        
        return super().skip_line(line)

    # Replace any line that starts @FORWARD_DECLARATION: with the actual declaration
    # This need to be called once all other functions have been converted to ensure the
    # signatures are correct
    def process_forward_declarations(self, static_funcsig_mappings):
        lines = self._cppfunction.code

        for i in range(len(lines)):
            m = re.match(rf"^@FORWARD_DECLARATION:(\w+)", lines[i])
            if m:
                processed = False
                for mapping in static_funcsig_mappings:
                    if mapping.cfuncsig.name == m.group(1):
                        lines[i] = f"{mapping.cppfuncsig.as_string()};"
                        debug.line("process_forward_declarations",f"@FORWARD_DECLARATION: Updated: {lines[i]}")
                        processed = True

                if not processed:
                    lines[i] = ""
                    debug.line("process_forward_declarations",f"@FORWARD_DECLARATION: Removed (not a static function) : {m.group(0)}")