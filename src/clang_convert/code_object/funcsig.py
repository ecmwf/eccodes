import debug
import code_object.arg as arg
import code_object.code_interface as code_interface

# Represent a function signature
#
# Note: Internally the return type and name are stored as the Arg _func_arg, as this allows
#       the return_type to be passed in as a string, with the Arg __init__ function providing
#       consistent parsing
class FuncSig(code_interface.CodeInterface):
    def __init__(self, return_type, name, args, template=None) -> None:
        self._func_arg = arg.Arg(return_type, name)
        self._args = args
        self._template = template

    @property
    def return_type(self):
        return self._func_arg.decl_spec.as_string()

    @property
    def name(self):
        return self._func_arg.name

    @property
    def args(self):
        return self._args

    @property
    def is_static(self):
        return self._func_arg.storage_class != ""

    @property
    def template(self):
        return self._template
    
    @template.setter
    def template(self, value):
        self._template = value

    def as_lines(self):
        return [f"{self._func_arg.as_string()}({', '.join([a.as_string() for a in self.args if a])})"]
