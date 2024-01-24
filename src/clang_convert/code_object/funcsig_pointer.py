import code_object.funcsig as funcsig
import code_object.arg as arg
from code_object.code_interface import NONE_VALUE

# Represent a function pointer using a function signature - it is basically the same, except the as_string()
# function produces the correct string...
#
# We're assuming the function pointer is a non-const "single" pointer - any other types will be
# converted incorrectly!
#
# Examples:
#
# GOOD: typedef int (*pfunc)        (int arg1, float arg2);
# BAD:  typedef int (**ppfunc)      (int arg1, float arg2);
# BAD:  typedef int (*const cpfunc) (int arg1, float arg2);
#
class FuncSigPointer(funcsig.FuncSig):
    def __init__(self, return_type, name, args) -> None:
        super().__init__(return_type, name, args)

    def as_lines(self):
        return [f"typedef {self._func_arg.decl_spec.as_string()} (*{self._func_arg.name})({', '.join([a.as_string() for a in self.args if a != NONE_VALUE])});"]
