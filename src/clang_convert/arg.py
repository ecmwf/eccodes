
import debug
from declaration_specifier import DeclSpec
import re

# Create an arg using a decl_specifier_seq string (see DeclSpec class) and (optional) name
# name can be "" to represent a type only
class Arg:
    # Note: decl_spec can be a string or an instance of class DeclSpec
    def __init__(self, decl_spec, name="", is_func_arg=False) -> None:
        if isinstance(decl_spec, DeclSpec):
            self._decl_spec = DeclSpec.from_instance(decl_spec)
        else:
            self._decl_spec = DeclSpec.from_decl_specifier_seq(decl_spec)
        self._name = name
        self._is_func_arg = is_func_arg # Support for processing function sig args differently to other args

    # Create from an existing Arg
    @classmethod
    def from_instance(cls, instance):
        return cls(instance.decl_spec, instance.name, instance.is_func_arg)

    # Use decl_spec to get storage_class etc...
    @property
    def decl_spec(self):
        return self._decl_spec

    @property
    def name(self):
        return self._name

    @property
    def is_func_arg(self):
        return self._is_func_arg

    # Support for Arg as a dict key
    def __hash__(self):
        return hash((self.decl_spec, self.name))

    # Support for Arg as a dict key
    def __eq__(self, other):
        if isinstance(other, Arg):
            return self.decl_spec == other.decl_spec and self.name == other.name
        return False

    def as_string(self):
        return self.decl_spec.as_string() + " " + self.name

# print helper - can be called when Arg is None
def arg_string(arg):
    if arg:
        return arg.as_string()
    
    return "None"
