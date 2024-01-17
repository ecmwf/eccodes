
import cpp_code_object.member_function as member_function
import utils.debug as debug
import re

# Representation of a C++ class member function
class ConstructorFunction(member_function.MemberFunction):
    def __init__(self, funcsig, body, class_name) -> None:
        super().__init__(funcsig, body, class_name)

    # Overridden to set name to class_name::
    @property
    def funcsig_as_declaration(self):
        funcsig_string = re.sub(r"^[^\(]*\(", rf"{self._class_name}(", self._funcsig.as_string())
        return funcsig_string

    # Overridden to set name to  class_name::class_name
    @property
    def funcsig_as_definition(self):
        funcsig_string = re.sub(r"^[^\(]*\(", rf"{self._class_name}::{self._class_name}(", self._funcsig.as_string())
        return funcsig_string
