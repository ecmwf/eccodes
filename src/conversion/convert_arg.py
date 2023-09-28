from convert_debug import debug_line
import re

# Represent an argument in the form TYPE NAME
# NAME can be "" to represent a type only
class Arg:
    def __init__(self, type, name="") -> None:
        self.type = type
        self.name = name

    # Create Arg from an input string 
    @classmethod
    def from_string(cls, input):
        # Note: "return x;" looks like a variable declaration, so we explicitly exclude this...
        # Note: We ignore const for now...
        m = re.match(r"(?:const)?\s*(\w+\**)\s+(\w+)\s*(\[\d*\])?", input)

        if m:
            arg_type = m.group(1)
            arg_name = m.group(2)
            if m.group(3):
                # Handle array declaration e.g. char buf[10]
                arg_type += m.group(3)

            debug_line("Arg from_string", f"Creating Arg: {arg_type} {arg_name} from input: {input}")
            return cls(arg_type, arg_name)
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

# print helper - can be called when Arg is None
def arg_string(arg):
    if arg:
        return arg.as_declaration()
    
    return "None"

def transform_variable_name(name):
    name_parts = name.split("_")

    if len(name_parts) > 1:
        name = name_parts[0] + "".join(x.capitalize() for x in name_parts[1:])
    
    return name[0].lower() + name[1:]

def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return transform_variable_name(name)

def transform_class_name(name):
    name = transform_variable_name(name)
    return name[0].upper() + name[1:]