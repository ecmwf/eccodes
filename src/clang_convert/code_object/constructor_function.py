
import code_object.member_function as member_function
import utils.debug as debug
import re
from code_object.code_interface import NONE_VALUE

# Representation of a C++ class member function
class ConstructorFunction(member_function.MemberFunction):
    def __init__(self, funcsig, body, class_name="", super_class_name="") -> None:
        super().__init__(funcsig, body, class_name)
        self._super_class_name = super_class_name

    @property
    def super_class_name(self):
        return self._super_class_name

    # Overridden to set name to class_name::
    @property
    def funcsig_as_declaration(self):
        funcsig_string = re.sub(r"^[^\(]*\(", rf"{self._class_name}(", self._funcsig.as_string())
        return funcsig_string

    # Overridden to set name to class_name::class_name
    @property
    def funcsig_as_definition(self):
        funcsig_string = re.sub(r"^[^\(]*\(", rf"{self._class_name}::{self._class_name}(", self._funcsig.as_string())
        funcsig_string += f": {self._super_class_name}({','.join([a.name for a in self._funcsig.args if a != NONE_VALUE])})"
        return funcsig_string
