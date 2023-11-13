import debug
import re

# Represent an argument in the form TYPE NAME
# NAME can be "" to represent a type only
class Arg:
    def __init__(self, type, name="") -> None:
        self.type = type
        self.name = name
        self._is_func_arg = False

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
        if self.type.startswith("std::string"):
            return "char"
        
        m = re.match(r"std::((vector)|(array))<(\w+)>", self.type)
        if m:
            return m.group(4)

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

    @property
    def is_func_arg(self):
        return self._is_func_arg

    # Create Arg from an input string 
    @classmethod
    def from_string(cls, input):
        arg_type, arg_name = parse_type_and_name_from_string(input)

        if arg_type and arg_name:
            return cls(arg_type, arg_name)

        debug.line("from_string", f"Input is not an arg declaration: {input}")
        return None

    # Create Arg from a function argument input string - this is similar to from_string(), except it will
    # accept just the type, so can parse unused function args that have no name (in this case name is None)
    @classmethod
    def from_func_arg_string(cls, input):
        arg_type, arg_name = parse_type_and_name_from_string(input)

        if arg_type:
            func_arg = cls(arg_type, arg_name)
            func_arg._is_func_arg = True
            debug.line("from_string", f"Creating function arg type=[{arg_type}] name=[{arg_name}] from: {input}")
            return func_arg

        debug.line("from_string", f"Input is not a function arg declaration: {input}")
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
    "AccessorDataBuffer",
]

# Return True if type is a container (std::vector etc)
# Will safely handle type that is None
def is_container_type(type):
    if not type:
        return False

    for c in container_types:
        if c in type:
            return True
    
    return False

# Return True if Arg is a container (std::vector etc)
# Will safely handle Arg that is None
def is_container(arg):
    if not arg:
        return False
    
    return is_container_type(arg.type)


# print helper - can be called when Arg is None
def arg_string(arg):
    if arg:
        return arg.as_declaration()
    
    return "None"

# Helper to extract the type and name from an input string - used by the Arg class
# Returns arg_type, arg_name as strings, or None if the input could no be parsed
def parse_type_and_name_from_string(input):

    # The parsing is split into two phases, type then name
    #
    # This is to support unused function parameters where no name is supplied
    #
    # Phase 1 - Type
    #
    # The regex is quite complicated, using the following groups:
    #
    # Groups: 1           2      3        4    5   6    
    # Groups: const(expr) struct unsigned TYPE PTR const
    #
    # Group 5 will match the pointer regardless of whether it is with the type or name, i.e.
    #   int* var;
    #   int *var;
    #   int** var;
    #   int **var;
    #
    # However to simplify future parsing, the pointer type will be stored as int*, not int *
    #
    # Phase 2 - Name
    #
    # If type is successfully extracted, then the name and index (if present, e.g. [2]) are parsed
    #
    # Notes: 
    #       1. "return x;" looks like a variable declaration, so we explicitly exclude this...
    #
    #       2. We ensure that AT LEAST ONE whitespace character must be matched after group 6 to ensure we
    #          have two separate identifiers for type and name
    #
    #       3. The index is actually added to the type to help with future parsing
    #

    input = tidy_arg_string(input)
    arg_type = arg_name = None

    # Phase 1 - type
    #m = re.match(r"(const(?:expr)?)?(struct)?\s*(unsigned)?\s*(\w+)(\s\*+|\*+\s?|\s)?(const)?", input)
    m = re.match(r"(const(?:expr)?)?(struct)?\s*(unsigned)?\s*(\w+)([\s\*]*)(const)?", input)
    if m:
        if m.group(4) in ["return", "typedef", "goto"]:
            debug.line("from_string", f"Ignoring invalid arg type [{m.group(4)}]: {input}")
            return None, None

        arg_type = ""
        if m.group(1):
            arg_type += m.group(1) + " "
        if m.group(2):
            arg_type += m.group(2) + " "
        if m.group(3):
            arg_type += m.group(3) + " "
        arg_type += m.group(4)
        if m.group(5):
            arg_type += m.group(5).replace(" ", "")  # Pointer string could be "* *" eg "T* *pval" in data_simple_packing
        if m.group(6):
            arg_type += " " + m.group(6)

        assert arg_type, f"Error extracting arg type from input=[{input}]"

    # Phase 2 - name
    if arg_type:
        m = re.match(r"\s*(\w+)\s*(\[\d*\])?", input[m.end():])
        if m:
            arg_name = m.group(1)

            if m.group(2):
                # Handle array declaration e.g. char buf[10]
                arg_type += m.group(2)
                debug.line("from_string", f"ARRAY: arg_type=[{arg_type}] arg_name=[{arg_name}] input=[{input}]")

            assert arg_name, f"Error extracting arg name from input=[{input}], type=[{arg_type}]"

    if arg_name is None:
        arg_name = ""

    return arg_type, arg_name

# Helper to strip out any comments and excess space
# from a potential arg strings...
def tidy_arg_string(arg_string):
    tidy_arg = ""
    match_start = match_end = 0
    m = re.search(r"/\*", arg_string)
    if m:
        match_start = m.start()
        match_end = m.end()
        m = re.search(rf"\*/", arg_string[match_end:])
        if m:
            match_end += m.end()

    tidy_arg = arg_string[:match_start] + arg_string[match_end:]
    tidy_arg = tidy_arg.strip()

    return tidy_arg
