
from method import *
import funcsig

destructor_default_line = "static void destroy(grib_context* c, grib_accessor* a)"

class DestructorMethod(Method):
    def __init__(self, func_sig=None, self_class=None) -> None:
        if not func_sig:
            func_sig = funcsig.FuncSig.from_string(destructor_default_line)
        super().__init__(func_sig, self_class)

