import code_object.code_interface as code_interface
import code_object.code_objects as code_objects

# Represents a macro instantiation, for example
#   BAR(42)
#   Assert(x != 4)
#
# name (str) is the Macro name
# expression (CodeObjects) is the contents of the Macro call (inside the parens)
class MacroInstantation(code_interface.CodeInterface):
    def __init__(self, name, expression):
        self._name = name
        self._expression = expression
        assert isinstance(self._name, str), f"Name must be a str, not [{type(self._name).__name__}]"
        assert isinstance(self._expression, code_objects.CodeObjects), f"Expression must be a CodeObjects class, not [{type(self._expression).__name__}]"

    @property
    def name(self):
        return self._name
    
    @property
    def expression(self):
        return self._expression
    
    def as_lines(self):
        text = f"{self._name}{self._expression.as_string()};"
        # Ensure it is all on one line!
        text = text.replace("\n", "")
        return [text]
