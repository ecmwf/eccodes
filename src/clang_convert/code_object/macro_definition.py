import utils.debug as debug
import code_object.code_interface as code_interface

# Represents a macro definition in the following forms:
#   #define FOO 7
#   #define BAR(x) (x)*42
#
# Can create with a single line, or multiple lines, of code (as strings)
class MacroDefinition(code_interface.CodeInterface):
    def __init__(self, code=None):
        self._macro_def_lines = []

        if code is not None:
            if isinstance(code, list):
                self.add_lines(code)
            else:
                self.add_line(code)

    def as_lines(self):
        return self._macro_def_lines

    def add_line(self, line):
        if line is not None:
            self._macro_def_lines.append(line)

    def add_lines(self, lines):
        if lines is not None:
            if isinstance(lines, code_interface.CodeInterface):
                self._macro_def_lines.extend(lines.as_lines())
            else:
                self._macro_def_lines.extend(lines)
