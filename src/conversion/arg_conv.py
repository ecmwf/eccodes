# Convert Arg from C to C++

import re
import debug
import arg

# Arg type of None means it should be deleted, so we return
# CARG_NOT_FOUND to signal that the search should continue...
CARG_NOT_FOUND = arg.Arg("cargNotFound", "__C_ARGUMENT_NOT_FOUND__")

class ArgConverter:
    def __init__(self, initial_carg):
        self._initial_carg = initial_carg
        self._transforms = None

    def transform_known_funcsig_types(self, carg):
        if not carg.is_func_arg:
            return CARG_NOT_FOUND

        for ctype, cpptype in self._transforms.funcsig_types.items():
            if ctype == carg.non_const_type:
                if cpptype is None:
                    return None
                else:
                    return arg.Arg(cpptype, transform_variable_name(carg.name))

        return CARG_NOT_FOUND

    def transform_known_types(self, carg):
        for ctype, cpptype in self._transforms.types.items():
            if ctype == carg.non_const_type:
                if cpptype is None:
                    return None
                else:
                    return arg.Arg(cpptype, transform_variable_name(carg.name))
                
        return CARG_NOT_FOUND

    def transform_arrays(self, carg):
        m = re.search(r"(\[\d*\])", carg.type)
        if m:
            match_text = carg.type[:m.start()]

            for ctype, cpptype in self._transforms.types.items():
                if ctype == match_text:
                    if cpptype is None:
                        return None
                    else:
                        match_text = cpptype
                        break

            m = re.match(r"const (.*)", match_text)
            if m:
                return arg.Arg(f"const std::vector<{m.group(1)}>", transform_variable_name(carg.name))
            else:
                return arg.Arg(f"std::vector<{match_text}>", transform_variable_name(carg.name))
                
        return CARG_NOT_FOUND

    def transform_structs(self, carg):
        m = re.search(r"struct (\w*)(\**)", carg.type)
        if m:
            for ctype, cpptype in self._transforms.types.items():
                if ctype == m.group(1) and cpptype:
                    return arg.Arg(cpptype+m.group(2), transform_variable_name(carg.name))
                
        return CARG_NOT_FOUND

    # Helper for transform_pointers - no need to call directly!
    def transform_pointers_for_func_args(self, carg):
        assert carg.is_func_arg, f"carg [{arg.arg_string(carg)}] is not a func arg!"

        cpparg = self.transform_known_funcsig_types(carg)
        if cpparg and cpparg == CARG_NOT_FOUND:
            cpparg = self.transform_known_types(carg)
        if cpparg and cpparg == CARG_NOT_FOUND:
            cpparg = self.default_transform(carg)

        # Change * to &
        cpparg.type = re.sub(r"\*", "&", cpparg.type)

        debug.line("transform_pointers_for_func_args", f"[{arg.arg_string(carg)}] -> [{arg.arg_string(cpparg)}]")
        return cpparg

    def transform_pointers(self, carg):
        # Return None for grib_accessor_* Note: this is *NOT* grib_accessor*
        if re.search(r"grib_accessor_", carg.type):
            return None

        m = re.search(r"(const)?\s*(\w*)(\*+)\s*(const)?", carg.type)
        if m:
            # Is the type actually a struct?
            for arg_key, arg_value in self._transforms.all_args.items():
                if arg_key.name == m.group(2) and arg_key.type == "struct":
                    # Yep - remove the pointer (we may consider using a smart pointer here?)
                    return arg.Arg(arg_value.name, transform_variable_name(arg_key.name))

            if carg.is_func_arg:
                return self.transform_pointers_for_func_args(carg)
            else:
                cpptype = f"std::vector<{m.group(2)}>"
                if m.group(3) == "**":
                    cpptype += "&"
                return arg.Arg(cpptype, transform_variable_name(carg.name))
                
        return CARG_NOT_FOUND

    # The fallback if no other transforms apply...
    def default_transform(self, carg):
        return arg.Arg(self._initial_carg.type, transform_variable_name(self._initial_carg.name))

    # Returns the equivalent C++ arg (name and type), which could be None
    # Note there are some differences in what is produced if the arg is a function arg...
    #
    def to_cpp_arg(self, transforms):
        self._transforms = transforms

        # Tidy up type and arg: basically change "int[10] val" to "int[] val"
        m = re.search(r"(\[\d*\])", self._initial_carg.type)
        if m:
            updated_carg = arg.Arg(self._initial_carg.type[:m.start()]+"[]", self._initial_carg.name)
        else:
            updated_carg = arg.Arg(self._initial_carg.type, self._initial_carg.name)

        updated_carg._is_func_arg = self._initial_carg.is_func_arg

        transform_funcs = [
            self.transform_known_funcsig_types,
            self.transform_known_types,
            self.transform_arrays,
            self.transform_structs,
            self.transform_pointers,
            self.default_transform,
        ]
        
        cpparg = None

        for transform_func in transform_funcs:
            cpparg = transform_func(updated_carg)
            if not (cpparg and cpparg == CARG_NOT_FOUND):
                break

        if cpparg:
            cpparg._is_func_arg = updated_carg.is_func_arg

        debug.line("to_cpp_arg", f"Result: [{arg.arg_string(self._initial_carg)}] -> [{arg.arg_string(cpparg)}] updated_carg=[{arg.arg_string(updated_carg)}]")

        return cpparg
        
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

