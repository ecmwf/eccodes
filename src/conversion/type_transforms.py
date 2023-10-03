# C to C++ type transforms

import re
from convert_debug import debug_line
from convert_arg import *

# Add any C to C++ type transforms required here
# These are also added programmatically
c_to_cpp_type_transforms = {
    "grib_accessor*"    : None,
    "grib_handle*"      : None,
    "grib_context*"     : None,
    "char*"             : "std::string",
    "char[]"            : "std::string",
    "grib_iarray*"      : "std::vector<long>"
}

def add_c_to_cpp_type_transform(ctype, cpptype):
    c_to_cpp_type_transforms[ctype] = cpptype

# Tidy up type and arg, basically change int[10] val to int[] val
def update_carg_format(carg):
    m = re.search(r"(\w*)(\[\d*\])", carg.type)
    if m:
        return Arg(m.group(1)+"[]", carg.name)

    return carg  
    

# Returns the equivalent C++ arg (name and type), which could be None
def to_cpp_arg(carg):

    updated_carg = update_carg_format(carg)

    # [1] Check for defined transforms
    for k, v in c_to_cpp_type_transforms.items():
        if k == updated_carg.type:
            if v is None:
                return None
            else:
                return Arg(v, transform_variable_name(updated_carg.name))

    # [2] Process any other array types
    m = re.match(r"(\w*)(\[\d*\])", updated_carg.type)
    if m:
        for k, v in c_to_cpp_type_transforms.items():
            if k == m.group(1):
                if v is None:
                    return None
                else:
                    return Arg(f"std::vector<{v}>", transform_variable_name(updated_carg.name))
                
        return Arg(f"std::vector<{m.group(1)}>", transform_variable_name(updated_carg.name))

    # [3] Process other mapped types
    for k, v in c_to_cpp_type_transforms.items():
        if k == updated_carg.type:
            if v is None:
                return None
            else:
                return Arg(v, transform_variable_name(updated_carg.name))

    # [3] Return None for grib_accessor_*
    m = re.match(r"grib_accessor_", carg.type)
    if m:
        return None

    # [4] Pointer types
    m = re.match(r"(\w*)\*", carg.type)
    if m:
        return Arg(f"std::vector<{m.group(1)}>", transform_variable_name(updated_carg.name))

    # [5] Everything else
    return Arg(carg.type, transform_variable_name(carg.name))

# Returns the equivalent C++ arg (name and type), which could be None
# The type is almost the same as for the function body, with the 
# following exceptions:
# - pointers are converted to references (not std::vector)
# - arrays are converted to std::vector references
def to_cpp_func_sig_arg(carg):
    
    # [1] Pointer types
    if carg.type[-1]  == "*":
        # Check for defined transforms
        for k, v in c_to_cpp_type_transforms.items():
            if k == carg.type:
                if v is None:
                    return None
                else:
                    return Arg(v+"&", transform_variable_name(carg.name))

        # Other pointers: removing * to avoid getting std::vector type back
        cpp_arg = to_cpp_arg(Arg(carg.type[:-1], carg.name))

        cpp_arg.type += "&"
        return cpp_arg

    # [2] Everything else
    cpp_arg = to_cpp_arg(carg)

    if cpp_arg and carg.type[-1] == "]":
        cpp_arg.type += "&"

    return cpp_arg
