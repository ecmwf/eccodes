
from func_conv import *
import global_func

# Specialisation of FunctionConverter for global functions
class GlobalFunctionConverter(FunctionConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return global_func.GlobalFunction(cppfuncsig)

    # If the line starts [FORWARD_DECLARATION] then it is a placeholder from the file parser
    # We need to change it to a forward declaration of the transformed function
    def special_function_transforms(self, line):
        m = re.match(rf"^\[FORWARD_DECLARATION\](\w+)", line)
        if m:
            debug.line("special_function_transforms",f"[FORWARD_DECLARATION] name={m.group(1)}")
            for cfuncsig, cppfuncsig in self._transforms.static_funcsigs.items():
                if cfuncsig.name == m.group(1):
                    line = f"{cppfuncsig.as_string()};"
                    debug.line("special_function_transforms",f"[FORWARD_DECLARATION] {line}")
                    return line

            line = ""
            debug.line("special_function_transforms",f"Not a static function forward declaration - removing line: {m.group(0)}")

        return super().special_function_transforms(line)
