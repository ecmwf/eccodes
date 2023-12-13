import debug

# Provides an interface for common functionality such as getting string representation
#
# All Classes representing code elements, such as Arg, StructArg, CFunction etc should implement
# this interface
class CodeInterface:

    # Get the textual representation as an array of strings
    # This should always be overridden!
    def as_lines(self):
        lines = [self.__class__.__name__]

        return lines

    # Get the textual representation as a single string (with line breaks)
    # This implementation should usually be sufficient for sub-classes...
    def as_string(self):
        lines = self.as_lines()
        return "\n".join(lines)

# Safe way to print a CodeInterface object even if it is None - useful for
# debug output
def as_debug_string(code_interface_inst):
    if code_interface_inst:
        return code_interface_inst.as_string()
    else:
        return "None"