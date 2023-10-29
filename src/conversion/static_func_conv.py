
from func_conv import *
import static_func

# Specialisation of FunctionConverter for global functions
class StaticFunctionConverter(FunctionConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return static_func.StaticFunction(cppfuncsig)

    def custom_cfunction_updates(self, line):
        for mapping in self._transforms.static_funcsig_mappings:
            line,count = re.subn(rf"\b{mapping.cfuncsig.name}\s*\(", f"{mapping.cppfuncsig.name}(", line)
            if(count):
                debug.line("custom_cfunction_updates", f"name={mapping.cfuncsig.name} [after ]: {line}")

        return super().custom_cfunction_updates(line)
