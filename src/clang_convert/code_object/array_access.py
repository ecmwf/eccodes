
import code_object.code_interface as code_interface

# Represents accessing an element in an array in the form: name[index]
#
# name and index must be a code_interface subclass
class ArrayAccess(code_interface.CodeInterface):
    def __init__(self, name, index) -> None:
        self._name = name
        self._index = index
        assert isinstance(self._name, code_interface.CodeInterface), f"Name must be a CodeInterface class"
        assert isinstance(self._index, code_interface.CodeInterface), f"Index must be a CodeInterface class"

    @property
    def name(self):
        return self._name

    @property
    def index(self):
        return self._index

    def as_lines(self):
        return [f"{self._name.as_string()}[{self._index.as_string()}]"]
