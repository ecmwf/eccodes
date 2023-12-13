import debug
import code_object.arg as arg
import code_object.code_interface as code_interface

# Represent a function call
#
# Note: Name and args are just strings...
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

    def add_arg(self, arg_entry):
        self._args.append(arg_entry)

    def as_lines(self):
        return [f"{self._name}({', '.join([a for a in self._args])})"]
