
from method import *
import funcsig

constructor_default_line = "static void init(grib_accessor* a, const long v, grib_arguments* params)"

class ConstructorMethod(Method):
    def __init__(self, func_sig=None) -> None:
        if not func_sig:
            func_sig = funcsig.FuncSig.from_string(constructor_default_line)
        super().__init__(func_sig)

