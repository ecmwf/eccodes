import utils.debug as debug
import code_object.code_interface as code_interface

# An object that holds lines of code not represented by a known class (Arg, Funcsig etc),
# providing a consistent interface e.g. when parsing/converting
#
# Code can be a single line or a list of lines
class CodeLines(code_interface.CodeInterface):
    def __init__(self, code=None):
        self._lines = []

        if code is not None:
            if isinstance(code, list):
                self.add_lines(code)
            else:
                self.add_line(code)

    def as_lines(self):
        return self._lines

    def add_line(self, line):
        if line is not None:
            self._lines.append(line)

    def add_lines(self, lines):
        if lines is not None:
            if isinstance(lines, code_interface.CodeInterface):
                self._lines.extend(lines.as_lines())
            else:
                self._lines.extend(lines)
