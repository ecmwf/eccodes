
import code_object.code_interface as code_interface
import code_object.code_lines as code_lines

# Represents accessing an element in an array in the form: name[index]
#
# name and index can be a string or a code_interface subclass
class ArrayAccess(code_interface.CodeInterface):
    def __init__(self, name, index) -> None:
        if isinstance(name, str):
            self._name = code_lines.CodeLines(name)
        else:
            self._name = name
        assert isinstance(self._name, code_interface.CodeInterface), f"Name must be a CodeInterface class (or a string)"

        if isinstance(index, str):
            self._index = code_lines.CodeLines(index)
        else:
            self._index = index
        assert isinstance(self._index, code_interface.CodeInterface), f"Index must be a CodeInterface class (or a string)"

    @property
    def name(self):
        return self._name

    @property
    def index(self):
        return self._index

    def as_lines(self):
        return [f"{self._name.as_string()}[{self._index.as_string()}]"]
