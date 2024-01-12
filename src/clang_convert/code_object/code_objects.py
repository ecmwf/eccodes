import code_object.code_interface as code_interface

# Represents a group of code objects (CodeInterface classes) that make up a coherent collection of code (e.g. a function body)
# Provides convenient ways of accessing the underlyting code
class CodeObjects:
    def __init__(self) -> None:
        self._code_objects = []

    @property
    def code_objects(self):
        return self._code_objects

    def add_code_object(self, item):
        assert isinstance(item, code_interface.CodeInterface), f"Item type=[{item.__class__}] is not a CodeInterface"
        self._code_objects.append(item)

    # Get the textual representation as an array of strings
    def as_lines(self):
        lines = []

        for entry in self._code_objects:
            lines.extend(entry.as_lines())

        return lines

    # Get the textual representation as a single string (with line breaks)
    def as_string(self):
        lines = self.as_lines()
        return "\n".join(lines)
    
