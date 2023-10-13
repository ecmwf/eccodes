
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
    def apply_function_transforms(self, line):
        m = re.search(rf"(?<!\")(&)?\b(\w+)\b(?!\")", line)
        if m:
            for cfuncname, cppfuncname in self._static_func_name_transforms.items():
                if m.group(2) == cfuncname:
                    prefix = m.group(1) if m.group(1) is not None else ""
                    line = re.sub(m.re, rf"{prefix}{cppfuncname}", line)
                    debug.line("apply_function_transforms", f"[Global Converter] Updating static function {m.group(0)} [after ]: {line}")

        return super().apply_function_transforms(line)

    # If the line starts [FORWARD_DECLARATION] then it is a placeholder from the file parser
    # We ignore it here, but will process it later (once everything else has been resolved)
    def special_function_transforms(self, line):
        m = re.match(rf"^\[FORWARD_DECLARATION\](\w+)", line)
        if m:
            debug.line("special_function_transforms",f"Ignoring (for now) [FORWARD_DECLARATION] name={m.group(1)}")

        return super().special_function_transforms(line)
    
    # Replace any line that starts [FORWARD_DECLARATION] with the actual declaration
    # This need to be called once all other functions have been converted to ensure the
    # signatures are correct
    def process_forward_declarations(self, static_funcsigs):
        lines = self._cppfunction.code

        for i in range(len(lines)):
            m = re.match(rf"^\[FORWARD_DECLARATION\](\w+)", lines[i])
            if m:
                processed = False
                for cfuncsig, cppfuncsig in static_funcsigs.items():
                    if cfuncsig.name == m.group(1):
                        lines[i] = f"{cppfuncsig.as_string()};"
                        debug.line("process_forward_declarations",f"[FORWARD_DECLARATION] Updated: {lines[i]}")
                        processed = True

                if not processed:
                    lines[i] = ""
                    debug.line("process_forward_declarations",f"[FORWARD_DECLARATION] Removed (not a static function) : {m.group(0)}")
