
import code_object.code_interface as code_interface
import utils.debug as debug
from code_object.declaration_specifier import DeclSpec
import re

# Create an arg using a decl_specifier_seq string (see DeclSpec class) and (optional) name
# name can be "" to represent a type only
class Arg(code_interface.CodeInterface):
    
    # Note: decl_spec can be a string or an instance of class DeclSpec
    def __init__(self, decl_spec, name="", is_func_arg=False) -> None:
        if isinstance(decl_spec, DeclSpec):
            self._decl_spec = DeclSpec.from_instance(decl_spec)
        elif isinstance(decl_spec, str):
            self._decl_spec = DeclSpec.from_decl_specifier_seq(decl_spec)
        else:
            assert False, f"Unexpected decl_spec=[{decl_spec}]"

        self._name = name
        self._is_func_arg = is_func_arg # Support for processing function sig args differently to other args

    # ---------- Support for NoneArg: Begin ----------

    # NoneArg is used when a C argument doesn't have an equivalent C++ argument
    class _NoneArg:
        def __repr__(self):
            return "<NoneArg>"

        def __eq__(self, other):
            # This version is required for testing Arg.NONE = <VAR>
            # As the Arg.__eq__ version won't be called when self=Arg.NONE !!!
            return isinstance(other, Arg._NoneArg)

        def __ne__(self, other):
            return not self.__eq__(other)

    NONE = _NoneArg()  # The singleton instance to represent "None"
    
    # ---------- Support for NoneArg: End   ----------

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

    # ---------- Support for Arg as a dict key: Begin ----------
    def __hash__(self):
        return hash((self.decl_spec, self.name))

    def __eq__(self, other):
        if self is Arg.NONE or other is Arg.NONE:
            return self is other
        return self.decl_spec == other.decl_spec and self.name == other.name

    def __ne__(self, other):
        return not self.__eq__(other)
    # ---------- Support for Arg as a dict key: End   ----------

    def as_lines(self):
        arg_string = self.decl_spec.as_string()
        if self.decl_spec.is_array_type():
            arg_string = arg_string.replace("[]", " ") + self.name + "[]"
        else:
            arg_string += " " + self.name
        return [arg_string]

