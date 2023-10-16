import debug
import re

# Represent an argument in the form TYPE NAME
# NAME can be "" to represent a type only
class Arg:
    def __init__(self, type, name="") -> None:
        self.type = type
        self.name = name

    # Support for Arg as a dict key
    def __hash__(self):
        return hash((self.type, self.name))

    # Support for Arg as a dict key
    def __eq__(self, other):
        if isinstance(other, Arg):
            return self.type == other.type and self.name == other.name
        return False
    
    # Return the "raw" type, e.g. char for char* or int for const int&
    @property
    def underlying_type(self):
        m = re.match(r"(?:const)?\s*(\w+)\s*[\*&]?\*?\s*(const)?", self.type)
        if m:
            return m.group(1)
        else:
            return self.type

    # Return the type without (first) const, e.g. "char" for "const char", "int* const" for "const int* const"
    @property
    def non_const_type(self):
        m = re.match(r"const\s*(.*)?", self.type)
        if m:
            return m.group(1)
        else:
            return self.type

    # Create Arg from an input string 
    @classmethod
    def from_string(cls, input):

        # Note: "return x;" looks like a variable declaration, so we explicitly exclude this...
        # Groups: 1     2      3    4 5     6   7
        # Groups: const struct TYPE * const NAME [N]
        m = re.match(r"(const)?(struct)?\s*(\w+)\s*(\*+)?\s*(const)?\s*(\w+)\s*(\[\d*\])?", input)

        if m:
            arg_type = ""
            if m.group(1):
                arg_type += m.group(1) + " "
            if m.group(2):
                arg_type += m.group(2) + " "
            arg_type += m.group(3)
            if m.group(4):
                arg_type += m.group(4)  # Add * if present...
            if m.group(5):
                arg_type += " " + m.group(5)  # Add * if present...

            arg_name = m.group(6)
            if m.group(7):
                # Handle array declaration e.g. char buf[10]
                arg_type += m.group(7)

            return cls(arg_type, arg_name)

        debug.line("Arg from_string", f"Couldn't create arg from input: {input}")
        return None
    
    # Generate a string to represent the Arg's declaration
    def as_declaration(self):
        arg_type = self.type
        arg_name = self.name
        # need to switch e.g. int[5] foo to int foo[5]
        m = re.match(r"(\w*)(\[\d*\])", arg_type)
        if m and m.group(2):
            arg_type = m.group(1)
            arg_name += m.group(2)
        
        return f"{arg_type} {arg_name}"
    
    def is_const(self):
        if re.search(r"\bconst\b", self.type):
            return True
        return False

container_types = [
    "std::string",
    "std::array",
    "std::vector",
    "std::map",
]
# Return True if Arg is a container (std::vector etc)
# Will safely handle Arg that is None
def is_container(arg):
    if not arg:
        return False
    
    for c in container_types:
        if arg.type.startswith(c):
            return True
    
    return False


# print helper - can be called when Arg is None
def arg_string(arg):
    if arg:
        return arg.as_declaration()
    
    return "None"
