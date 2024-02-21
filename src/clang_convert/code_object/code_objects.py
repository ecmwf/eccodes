import code_object.code_interface as code_interface
import code_object.code_objects as code_objects
from utils import debug

# Represents a group of code objects (CodeInterface classes) that make up a coherent collection of code (e.g. a function body)
# Provides convenient ways of accessing the underlying code
class CodeObjects(code_interface.CodeInterface):
    def __init__(self) -> None:
        self._code_objects = []

    @property
    def code_objects(self):
        return self._code_objects

    def add_code_object(self, item):
        assert isinstance(item, code_interface.CodeInterface), f"Item type=[{item.__class__}] is not a CodeInterface"
        self._code_objects.append(item)

    def add_code_object_collection(self, collection):
        assert isinstance(collection, code_objects.CodeObjects), f"Collection type=[{collection.__class__}] is not a CodeObjects instance"
        for entry in collection.code_objects:
            self.add_code_object(entry)

    # Get the textual representation as an array of strings
    def as_lines(self):
        lines = []

        for entry in self._code_objects:
            lines.extend(entry.as_lines())

        return lines
    