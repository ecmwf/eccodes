
import code_object.function as function
import utils.debug as debug
import re

# Representation of a C++ class member function
class MemberFunction(function.Function):
    def __init__(self, funcsig, body, class_name="") -> None:
        super().__init__(funcsig, body)
        self._class_name = class_name

        self._const = False
        self.using = []

    @property
    def class_name(self):
        return self._class_name
    
    @class_name.setter
    def class_name(self, name):
        self._class_name = name

    # Overridden to add class_name::
    @property
    def funcsig_as_definition(self):
        funcsig_string = re.sub(r"^(.*\s)([^\(]*\()", rf"\1{self._class_name}::\2", self._funcsig.as_string())
        return funcsig_string

    @property
    def const(self):
        return "const" if self._const else ""

    @const.setter
    def const(self, value):
        self._const = value