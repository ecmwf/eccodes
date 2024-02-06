
import code_object.arg as arg
from code_object.code_interface import NONE_VALUE

# A class data member - modelled as an Arg with a few extra bits!
class DataMember(arg.Arg):
    
    # Note: decl_spec can be a string or an instance of class DeclSpec
    def __init__(self, decl_spec, name="", mutable=False) -> None:
        super().__init__(decl_spec, name)

        self.default_value = ""
        self._mutable = mutable

    @property
    def mutable(self):
        return self._mutable

    # Create from an existing Arg
    @classmethod
    def from_instance(cls, instance):
        return cls(instance.decl_spec, instance.name, instance.mutable)

    # ---------- Support for DataMember as a dict key: Begin ----------
    def __hash__(self):
        return hash((self.decl_spec, self.name))

    def __eq__(self, other):
        if self is NONE_VALUE or other is NONE_VALUE:
            return self is other

        # Ensure the other object is an instance of DataMember for comparison.
        if not isinstance(other, DataMember):
            return False

        return self.decl_spec == other.decl_spec and self.name == other.name
    # ---------- Support for DataMember as a dict key: End   ----------

    def as_lines(self):
        arg_string = "mutable " if self.mutable else ""
        arg_string += self.decl_spec.as_string()
        if self.decl_spec.is_array_type():
            arg_string = arg_string.replace(self.decl_spec.pointer, " ") + self.name + self.decl_spec.pointer
        else:
            arg_string += " " + self.name
        return [arg_string]
