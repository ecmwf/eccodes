import utils.debug as debug

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

    # ---------- Support for NoneValue: Begin ----------

    # NoneValue is used when a C argument doesn't have an equivalent C++ argument
    class _NoneValue:
        def __repr__(self):
            return "<NoneValue>"

        def __eq__(self, other):
            # This version is required for testing NONE_VALUE = <VAR>
            # As the CodeInterface.__eq__ version won't be called when self=NONE_VALUE !!!
            return isinstance(other, CodeInterface._NoneValue)

        def __ne__(self, other):
            return not self.__eq__(other)
        
        # Support for use as a dict key
        def __hash__(self):
            return hash("<NoneValue>")
        

    NONE = _NoneValue()  # The singleton instance to represent "None"
    
    def __eq__(self, other):
        if self is CodeInterface.NONE or other is CodeInterface.NONE:
            return self is other
        return isinstance(other, CodeInterface)

    def __ne__(self, other):
        return not self.__eq__(other)
    # ---------- Support for NoneValue: End   ----------

    # Get the textual representation as a single string (with line breaks)
    # This implementation should usually be sufficient for sub-classes...
    def as_string(self):
        lines = self.as_lines()
        return "\n".join(lines)

# Shortcut form of code_interface. CodeInterface. NONE
NONE_VALUE = CodeInterface.NONE
