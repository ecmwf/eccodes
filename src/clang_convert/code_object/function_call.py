import utils.debug as debug
import code_object.arg as arg
import code_object.code_interface as code_interface
import code_object.value_declaration_reference as value_declaration_reference
import code_object_converter.conversion_pack.arg_utils as arg_utils
from utils.string_funcs import strip_semicolon
from code_object.code_interface import NONE_VALUE

# Represent a function call
#
# Note: Name is a string; args are CodeInterface instances
class FunctionCall(code_interface.CodeInterface):
    def __init__(self, name, args=None) -> None:
        self._name = name
        self._args = [] if args is None else args

    @property
    def name(self):
        return self._name

    @property
    def args(self):
        return self._args

    # return the arguments as a comma-separated list
    @property
    def arg_string(self):
        return ",".join([strip_semicolon(a.as_string()) for a in self._args])

    def add_arg(self, arg_entry):
        assert isinstance(arg_entry, code_interface.CodeInterface), f"arg_entry must be a CodeInterface class, supplied=[{arg_entry}]"
        self._args.append(arg_entry)


    def as_lines(self):
        func_call_string = f"{self._name}({', '.join([arg_utils.extract_function_call_name(a) for a in self._args])});"
        debug.line("as_lines", f"func_call_string=[{func_call_string}]")
        return [func_call_string]
        #return [f"{self._name}({', '.join([strip_semicolon(a.as_string()) for a in self._args])});"]
