import code_object.macro_definition as macro_definition

# Represents a macro instantiation, for example
#   BAR(42)
#   Assert(x != 4)
#
# Can create with a single line, or multiple lines, of code (as strings)
#
# For now, derive from macro_definition as the (current) implementation is identical!
class MacroInstantation(macro_definition.MacroDefinition):
    def __init__(self, code=None):
        super().__init__(code)
