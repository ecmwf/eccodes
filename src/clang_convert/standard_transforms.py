
import re

# Standard C to C++ transforms

# Convert C-style snake_case type name to C++-style PascalCase
def transform_type_name(name):
    if not name:
        return name

    name_parts = name.split("_")

    if len(name_parts) > 1:
        return "".join(x.capitalize() for x in name_parts)
    else:
        return name[0].upper() + name[1:]

# Change C-style snake_case variable name to C++-style camelCase
def transform_variable_name(name):
    if not name:
        return name
    
    name = transform_type_name(name)
    
    return name[0].lower() + name[1:]

# Change C-style snake_case function name to C++-style camelCase name
# Also, remove get_ and set_ prefixes
def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return transform_variable_name(name)
