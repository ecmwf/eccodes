# Convert Arg from C to C++

import re
import debug
import arg

class ArgConverter:
    def __init__(self, carg):
        self._carg = carg

    # Returns the equivalent C++ arg (name and type), which could be None
    def to_cpp_arg(self, transforms):
        updated_carg = update_carg_format(self._carg)

        # [1] Check for defined transforms
        for k, v in transforms.types.items():
            if k == updated_carg.non_const_type:
                if v is None:
                    return None
                else:
                    return arg.Arg(v, transform_variable_name(updated_carg.name))

        # [2] Process any other array types
        m = re.search(r"(\w*)(\[\d*\])", updated_carg.type)
        if m:
            for k, v in transforms.types.items():
                if k == m.group(1):
                    if v is None:
                        return None
                    else:
                        return arg.Arg(f"std::vector<{v}>", transform_variable_name(updated_carg.name))
                    
            return arg.Arg(f"std::vector<{m.group(1)}>", transform_variable_name(updated_carg.name))

        # [3] Process other mapped types
        for k, v in transforms.types.items():
            if k == updated_carg.non_const_type:
                if v is None:
                    return None
                else:
                    return arg.Arg(v, transform_variable_name(updated_carg.name))

        # [3] Return None for grib_accessor_*
        m = re.search(r"grib_accessor_", self._carg.type)
        if m:
            return None

        # [4] Struct (Pointer) types
        m = re.search(r"struct (\w*)(\**)", self._carg.type)
        if m:
            for k, v in transforms.types.items():
                if k == m.group(1) and v:
                    return arg.Arg(v+m.group(2), transform_variable_name(updated_carg.name))

        # [5] Pointer types
        m = re.search(r"(\w*)(\*+)", self._carg.type)
        if m:
            # Is the type actually a struct?
            for k,v in transforms.all_args.items():
                if k.name == m.group(1) and k.type == "struct":
                    # Yep - remove the pointer (we may consider using a smart pointer here?)
                    return arg.Arg(v.name, transform_variable_name(updated_carg.name))

            cpptype = f"std::vector<{m.group(1)}>"
            if m.group(2) == "**":
                cpptype += "&"
            return arg.Arg(cpptype, transform_variable_name(updated_carg.name))

        # [5] Everything else
        return arg.Arg(self._carg.type, transform_variable_name(self._carg.name))

    # Returns the equivalent C++ arg (name and type), which could be None
    # The type is almost the same as for the function body, with the 
    # following exceptions:
    # - pointers are converted to references (not std::vector)
    # - arrays are converted to std::vector references
    def to_cpp_func_sig_arg(self, transforms):
        
        # [1] Pointer types
        m = re.search(r"(const)?\s*(\w*)(\*+)\s*(const)?", self._carg.type)
        if m:
            # Check for defined transforms (funcsig first, then everything else)
            for type_dict in [transforms.funcsig_types, transforms.types]:
                for k, v in type_dict.items():
                    if k == self._carg.non_const_type:
                        if v is None:
                            return None
                        else:
                            cpparg = arg.Arg(v+"&", transform_variable_name(self._carg.name))
                            return cpparg

            # Other pointers: removing * to avoid getting std::vector type back (unless it's **)
            test_type = m.group(2)
            if m.group(3) and len(m.group(3)) > 1:
                test_type += m.group(3)[:-1]
            self._carg = arg.Arg(test_type, self._carg.name)
            cpparg = self.to_cpp_arg(transforms)

            if cpparg:
                if m.group(1):
                    cpparg.type += " const"
                cpparg.type += "&"
                
            return cpparg

        # [2] Everything else
        cpparg = self.to_cpp_arg(transforms)

        if cpparg and self._carg.type[-1] == "]":
            cpparg.type += "&"

        return cpparg


# Tidy up type and arg: basically change int[10] val to int[] val
def update_carg_format(carg):
    m = re.search(r"(\w*)(\[\d*\])", carg.type)
    if m:
        return arg.Arg(m.group(1)+"[]", carg.name)

    return carg  

# Convert C-style snake_case type name to C++-style PascalCase
def transform_type_name(name):
    name_parts = name.split("_")

    if len(name_parts) > 1:
        return "".join(x.capitalize() for x in name_parts)
    else:
        return name[0].upper() + name[1:]

# Change C-style snake_case variable name to C++-style camelCase
def transform_variable_name(name):
    name = transform_type_name(name)
    
    return name[0].lower() + name[1:]

