import code_object.code_objects as code_objects

# Represents a block of code enclosed in parentheses
#
# Stored as code objects (CodeInterface classes)
class CompoundStatement(code_objects.CodeObjects):

    # Get the textual representation as an array of strings
    def as_lines(self):
        lines = ["{"]

        for entry in self._code_objects:
            lines.extend(entry.as_lines())

        lines.append("}")

        return lines
