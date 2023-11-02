# Convert C function to C++
import debug
import func
import arg
import arg_conv
import c_subs
import re
import grib_api_converter
import struct_arg
import variable

# Change C-style snake_case function name to C++-style camelCase name
# Also, remove get_ and set_ prefixes
def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return arg_conv.transform_variable_name(name)

# Generic function converter - overridden as required
class FunctionConverter:
    def __init__(self):
        self._in_comment_block = False
        self._transforms = None
        self._cfunction = None # Set whilst processing...
        self._cppfunction = None # Set whilst processing...

    @property
    def transforms(self):
        return self._transforms
    
    # Convert cfunction to cppfunction
    # This is the main entry point and should not be overridden - override
    # the specific function calls within as required...
    def to_cpp_function(self, cfunction, transforms):
        self._transforms = transforms
        self._transforms.clear_local_args()

        self._cfunction = cfunction
        cppfuncsig = self._transforms.cppfuncsig_for(self._cfunction.name)

        # Store the C to C++ function arg transforms
        for index, carg in enumerate(self._cfunction.func_sig.args):
            assert carg, f"carg none for function name=[{cfunction.name}]"

            cpparg = cppfuncsig.args[index]
            self._transforms.add_local_args(carg, cpparg)

        self._cppfunction = self.create_cpp_function(cppfuncsig)
        for line in self.create_cpp_body():
            self._cppfunction.add_line(line)
        
        return self._cppfunction

    def create_cpp_function(self, cppfuncsig):
        assert False, "[WARNING] Unexpected call to FunctionConverter instance - consider overriding!"
        return func.Function(cppfuncsig)

    # Return true if the line should be skipped
    # Override as required
    def skip_line(self, line):
        if not line:
            debug.line("skip_line", f"[Empty]: {line}")
            return True

        # Ignore multi-line macros (for now)
        if line.endswith("\\"):
            debug.line("skip_line", f"[Multi-line Macro]: {line}")
            return True

        if re.match(r"^\s*//", line):
            debug.line("skip_line", f"[C++ Comment]: {line}")
            return True

        # NOTE: /* only matches at the START of the line, and is ignored if it appears after code
        m = re.match(r"^\s*(/\*)?.*?(\*/)?$", line)

        if m:
            if m.group(1) and not m.group(2):   # /*
                debug.line("skip_line", f"[C Comment Start]: {line}")
                self._in_comment_block = True
                return True
            
            elif m.group(1) and m.group(2):     # /* */
                self._in_comment_block = False
                debug.line("skip_line", f"[C Comment]: {line}")
                return True

            elif not m.group(1) and m.group(2): #    */
                if self._in_comment_block:
                    self._in_comment_block = False
                    debug.line("skip_line", f"[C Comment End  ]: {line}")
                    return True

        if self._in_comment_block:
            debug.line("skip_line", f"[C Comment Block]: {line}")
            return True

        return False

    # ======================================== UPDATE FUNCTIONS ========================================

    # TODO: Move to grib_api folder...
    def convert_grib_utils(self, line):
        for util in ["grib_is_earth_oblate",]:
            m = re.search(rf"\b{util}\b", line)
            if m:
                cpp_util = transform_function_name(util)
                line = re.sub(m.re, f"{cpp_util}", line)
                debug.line("convert_grib_utils", f"Replaced {util} with {cpp_util} [after ]: {line}")

        return line
    
    def update_grib_api_cfunctions(self, line):
        line = self.convert_grib_utils(line)
        line = grib_api_converter.convert_grib_api_functions(line)

        return line


    # Returns the cppfunction_name for the supplied cfunction_name, or None
    # Override as required...
    def transform_cfunction_name(self, cfunction_name):
        # Static function substitutions
        for mapping in self._transforms.static_funcsig_mappings:
            if cfunction_name == mapping.cfuncsig.name:
                return mapping.cppfuncsig.name
            
        # Check function pointers too - need to do this in two phases
        # Example using: err = codec_element(..) [grib_accessor_class_bufr_data_array.cc]
        # 1. Is codec_element in arg map? Yes: codec_element_proc codec_element
        # 2. Is codec_element_proc in the function pointers map? Yes: transform name to C++ name
        # Result: codecElement(...)
        for carg, cpparg in self._transforms.all_args.items():
            if cfunction_name == carg.name:
                for ctype in self._transforms.function_pointers.keys():
                    if ctype == carg.type:
                        return cpparg.name

        return None
    
    # Recursively transform the function names in the supplied line...
    def process_cfunction_names(self, line, depth):
        assert depth<15, f"Unexpected recursion depth [{depth}]"

        # Note: (?=\() ensures group(3) is followed by a "(" without capturing it - ensuring it's a function name!
        m = re.search(rf"(&)?\b(\w+)(?=\()", line)
        #m = re.search(rf"(&)?\b([a-zA-Z]\w*)", line)
        if m:
            prefix = m.group(1) if m.group(1) is not None else ""
            cfunction_name = m.group(2)

            # Process the remainder (recursively), so we can rebuild the line correctly from the end...
            remainder = self.process_cfunction_names(line[m.end():], depth+1)

            cppfunction_name = self.transform_cfunction_name(cfunction_name)

            if cppfunction_name:
                line = line[:m.start()] + prefix + cppfunction_name + remainder
                debug.line("process_cfunction_names", f"[{depth}] Updated function name [{cfunction_name}] -> [{cppfunction_name}]: {line}")
            else:
                line = line[:m.end()] + remainder

        return line

    # General updates, then calls process_cfunction_names to update all function names in the line
    # Override as required
    def update_cfunction_names(self, line):
        line = c_subs.apply_all_substitutions(line)

        line = self.process_cfunction_names(line, 0)
        return line

    # Override this to provide any function updates specific to a class
    def custom_cfunction_updates(self, line):
        return line

    # Special transforms for well-known return types
    def transform_return_carg(self, carg):
        if carg.name in ["err", "ret"]:
            return arg.Arg("GribStatus", arg_conv.transform_variable_name(carg.name))
        else:
            return None

    # Returns the C++ equivalent, or None if var should be deleted
    def transform_carg(self, carg):
        cpparg = self.transform_return_carg(carg)

        if not cpparg:
            arg_converter = arg_conv.ArgConverter(carg)
            cpparg = arg_converter.to_cpp_arg(self._transforms)

        return cpparg

    # Find C variable declarations e.g. size_t len; char* buf = "Data"; and then:
    # 1. Transform to C++ and store in the arg_map
    # 2. Delete the line if no longer valid
    # Note: Assignments are not processed at this stage...
    def update_cvariable_declarations(self, line):

        m = re.match(rf"^(?:static)?\s*([^=;]*)[=;]", line)

        if not m:
            return line
        
        carg = arg.Arg.from_string(m.group(1))
        if not carg:
            return line

        cpparg = self.transform_carg(carg)

        # Store even if C++ is same as C so we can further process it later...        
        self._transforms.add_local_args(carg, cpparg)
        debug.line("update_cvariable_declarations", f"Added local arg: {arg.arg_string(carg)} -> {arg.arg_string(cpparg)}")

        if not cpparg:
            debug.line("update_cvariable_declarations", f"--> deleted line: {line}")
            return ""

        if carg != cpparg:
            line = re.sub(rf"{re.escape(m.group(1))}", f"{cpparg.as_declaration()}", line)
            debug.line("update_cvariable_declarations", f"--> updated line: {line}")

        return line

    # Find type declarations and store in the transforms
    def update_cstruct_type_declarations(self, line):

        # struct declaration [1]: [typedef] struct S [S]
        m = re.match(r"^(typedef)?\s*struct\s+(\w+)\s*(\2)?(;)?$", line)
        if m:
            if m.group(1) and m.group(4):
                line = "// [Removed struct typedef] " + line
            else:
                ctype = m.group(2)
                cpptype = arg_conv.transform_type_name(ctype)

                self._transforms.add_to_types(ctype, cpptype)

                line = re.sub(m.re, f"struct {cpptype}", line)
                debug.line("update_cstruct_type_declarations", f"Added struct type transform: {ctype} -> {cpptype} [after ]: {line}")

                carg = arg.Arg("struct", ctype)
                cpparg = arg.Arg("struct", cpptype)
                self._transforms.add_local_args(carg, cpparg)
                debug.line("update_cstruct_type_declarations", f"Adding struct to local arg map: {carg.type} {carg.name} -> {cpparg.type} {cpparg.name} [after ]: {line}")

            return line
        
        # struct declaration [2]: } S
        m = re.match(r"^}\s*(\w+)", line)
        if m and m.group(1) not in ["else"]:
            # We'll assume this definition is covered by [1]
            line = re.sub(m.re, "}", line)
            debug.line("update_cstruct_type_declarations", f"Removed extraneous struct definition: {m.group(1)} [after ]: {line}")

            return line

        # Check if this is a member pointer
        m = re.match(r"^\s*struct\s+(\w+)\s*(\*+\w+)?(;)?$", line)
        if m:
            line = re.sub(m.re, rf"\1 \2 \3", line)
            debug.line("update_cstruct_type_declarations", f"Removed struct from member definition: {m.group(1)} [after ]: {line}")

        return line

    # Special handling for function pointers: [typedef] RET (*PFUNC)(ARG1, ARG2, ..., ARGN);
    def update_cfunction_pointers(self, line):

        m = re.match(r"^(typedef )?\s*([^(]+\**)\s*\(\s*\*(\s*\w+)\s*\)\s*\((.*)\)", line)
        if m:
            carg = arg.Arg(m.group(2), m.group(3))
            arg_converter = arg_conv.ArgConverter(carg)
            cpparg = arg_converter.to_cpp_arg(self._transforms)

            # Assume functions returning int will now return GribStatus
            if cpparg.type.strip() == "int":
                cpparg.type = "GribStatus"

            debug.line("update_cfunction_pointers", f"Adding var to local arg map: {carg.type} {carg.name} -> {cpparg.type} {cpparg.name} [after ]: {line}")
            self._transforms.add_local_args(carg, cpparg)

            # Store the function pointer cname -> cppname as a type transform
            self._transforms.add_to_function_pointers(carg.name, cpparg.name)

            # Parse the function arg types
            cpp_arg_types = []
            for arg_string in [a.strip() for a in m.group(4).split(",")]:
                c_sig_arg = arg.Arg.from_func_arg_string(arg_string)
                arg_converter = arg_conv.ArgConverter(c_sig_arg)
                cpp_sig_arg = arg_converter.to_cpp_arg(self._transforms)
                if cpp_sig_arg:
                    cpp_arg_types.append(cpp_sig_arg.type)
            
            # Apply the transform
            line = re.sub(m.re, f"{m.group(1)}{cpparg.type}(*{cpparg.name})({','.join([a for a in cpp_arg_types])})", line)
            debug.line("update_cfunction_pointers", f"Transformed line: {line}")

        return line
    
    # Return None to delete the line
    def transform_cstruct_arg_name(self, cstruct_arg):
        assert cstruct_arg and cstruct_arg.name, f"Unexpected empty (None) cstruct_arg"

        for carg, cpparg in self._transforms.all_args.items():
            if carg.name == cstruct_arg.name:
                return cpparg.name if cpparg else None

        return cstruct_arg.name

    # If all else fails, use this!
    # Conversion is *foo[1]->bar[3]->baz[7] => foo[1].bar[3].baz[7]
    def apply_default_cstruct_arg_transform(self, cstruct_arg):
        cppstruct_arg = None
        if cstruct_arg:
            cppstruct_access = ""
            cppstruct_name = self.transform_cstruct_arg_name(cstruct_arg)
            cppstruct_index = cstruct_arg.index
            cppstruct_arg = struct_arg.StructArg(cppstruct_access, cppstruct_name, cppstruct_index)

            cmember = cstruct_arg.member
            cppmember = cppstruct_arg
            while cmember:
                cppstruct_access = "."
                cppstruct_name = self.transform_cstruct_arg_name(cmember)
                cppstruct_index = cmember.index

                cppmember.member = struct_arg.StructArg(cppstruct_access, cppstruct_name, cppstruct_index)
                cmember = cmember.member
                cppmember = cppmember.member

        return cppstruct_arg

    # If the type is well-known (e.g. grib_darray) then apply the transform
    # using the supplied cppname and the appropriate rules.
    # Returns the transformed cppstruct_arg, or None if ctype not recognised
    def apply_cstruct_arg_transforms_for_ctype(self, ctype, cstruct_arg, cppname):
        # Check if grib_array
        cppstruct_arg = grib_api_converter.process_cstruct_arg_for_grib_api_ctype(ctype, cstruct_arg, cppname)

        return cppstruct_arg

    # Transform cstruct_arg to cppstruct_arg
    # This is the main customisation point for sub-classes
    def transform_cstruct_arg(self, cstruct_arg):
        cppstruct_arg = None

        for carg, cpparg in self._transforms.all_args.items():
            if cstruct_arg.name == carg.name and cpparg:
                cppstruct_arg = self.apply_cstruct_arg_transforms_for_ctype(carg.type, cstruct_arg, cpparg.name)

        if not cppstruct_arg:
            cppstruct_arg = self.apply_default_cstruct_arg_transform(cstruct_arg)

        return cppstruct_arg

    # Update the remainder (of the current line) to correctly set the cppstruct_arg - returns the updated remainder
    def update_cppstruct_arg_assignment(self, cppstruct_arg, remainder):

        # Default - do nothing!
        return remainder

    # Takes a cppstruct and updates any C structs within the [] of the index...
    # Required because C to C++ struct transforms ignore the index contents and so miss any other structs...
    def update_cppstruct_indexes(self, cppstruct_arg):

        debug.line("update_cppstruct_indexes", f"IN  cppstruct_arg=[{cppstruct_arg.as_string()}]")

        struct_arg = cppstruct_arg
        while struct_arg:
            if struct_arg.index:
                struct_arg.index = self.update_cstruct_access(struct_arg.index, 0)
            struct_arg = struct_arg.member

        debug.line("update_cppstruct_indexes", f"OUT cppstruct_arg=[{cppstruct_arg.as_string()}]")

        return cppstruct_arg

    def update_cstruct_access(self, line, depth):
        assert depth<10, f"Unexpected recursion depth [{depth}]"

        debug.line("update_cstruct_access", f"IN [{depth}] line=[{line}]")

        cstruct_arg, match_start, match_end = struct_arg.cstruct_arg_from_string(line)
        if cstruct_arg:
            if depth == 0: debug.line("update_cstruct_access", f"IN cstruct_arg=[{cstruct_arg.as_string()}] : {line}")
            if(match_end < len(line)):
                # Recurse, transforming the remainder, then apply here...
                remainder = self.update_cstruct_access(line[match_end:], depth+1)

            cppstruct_arg = self.transform_cstruct_arg(cstruct_arg)
            assert cppstruct_arg, f"Could not transform struct {cstruct_arg.name} to C++"

            cppstruct_arg = self.update_cppstruct_indexes(cppstruct_arg)

            if not cppstruct_arg.name:
                # TODO: Only delete line if assignment, else just remove struct...
                line = f"// [Deleted struct {cstruct_arg.name}] " + line
                debug.line("update_cstruct_access", f"[{depth}] Deleting {cstruct_arg.name} Line: {line}")
                return line

            # Check if this is a "set" operation (it's match group 3 to avoid false match with ==)
            m = re.search(r"([=!\<\>\+\-\*/]=)|([,;\)])|(=)", line[match_end:])
            if m and m.group(3):
                match_end += m.end()
                remainder = " = " + self.update_cppstruct_arg_assignment(cppstruct_arg, line[match_end:])

            # Finally, update the line
            line = line[:match_start] + cppstruct_arg.as_string() + remainder

            if depth == 0: debug.line("update_cstruct_access", f"OUT : {line}")

        return line

    def update_cstruct_variables(self, line):
        line = self.update_cstruct_access(line, 0)
        return line

    # Checks if the supplied cvariable represents a known variable, and returns a 
    # transformed C++ variable, or None
    #
    # "Known variables" are determined by comparing the cvariable name to the stored lists.
    # If the cvariable name is found, the return value is a cpp variable with the components
    # updated (which may be set to None if no transform available). For example:
    #
    # cvariable             cname    -> cppname     cppvariable
    # (*, foo_name, "")     foo_name -> fooName     ("", fooName, "")
    # (*, handle, "")       handle   -> None        (None, None, None)
    # (*, bar, "")          NOT FOUND               None
    #
    # Note 1: We assume *var and &var should be replaced with var
    # Note 2: When a transformed cppvariable is returned, it has its type set as well (in case this is useful!)
    #
    def transform_if_cvariable(self, cvariable):
        for carg, cpparg in self._transforms.all_args.items():
            if cvariable.name == carg.name:
                if cpparg:
                    cppvariable = variable.Variable("", cpparg.name, cvariable.index)
                    cppvariable.type = cpparg.type
                    return cppvariable
                else:
                    return variable.Variable(None, None, None)

        return None

    # Checks if the supplied cvariable actually represents a function name, and returns the 
    # transformed C++ variable, or None
    #
    # Typical use-case is function pointers in initializer lists
    def transform_if_cfunction_name(self, cvariable):
        cppfunction_name = self.transform_cfunction_name(cvariable.name)

        if cppfunction_name:
            cppvariable = variable.Variable(cvariable.pointer, cppfunction_name, cvariable.index)
            debug.line("transform_if_cfunction_name", f"cvariable [{cvariable.as_string()}] is function - cppvariable=[{cppvariable.as_string()}]")
            return cppvariable

        return None

    # Update the remainder (of the current line) to correctly set the cppvariable - returns the updated remainder
    def update_cppvariable_assignment(self, cppname, remainder):

        # Default - do nothing!
        return remainder

    # Called first from transform_cvariable_access - override to provide specialised transforms
    def custom_transform_cvariable_access(self, cvariable, match_token, post_match_string):
        return None

    # Special transforms for return variables
    def transform_return_cvariable_access(self, cvariable, match_token, post_match_string):
        ret = "GribStatus"

        cppvariable = self.transform_if_cvariable(cvariable)

        if cppvariable and cppvariable.type == ret:
            # If match is e.g. "err)" then we'll assume it's a boolean test eg if(err) and not the last arg of a function call, so
            # we'll update it to a comparison
            if match_token.value == ")":
                transformed_string = cppvariable.as_string() + f" != {ret}::SUCCESS" + match_token.as_string() + post_match_string
                debug.line("transform_return_cvariable_access", f"transformed boolean return value test: {transformed_string}")
                return transformed_string
            
            elif match_token.is_separator or match_token.is_terminator:
                # Just need to transform the name
                transformed_string = cppvariable.as_string() + match_token.as_string() + post_match_string
                debug.line("transform_return_cvariable_access", f"return value transformed: {transformed_string}")
                return transformed_string
            
            else:
                # Assignment/Comparison, so need to ensure the "rvalue" is the correct type

                # Check if assigned to a function call - we assume the function returns the correct type!
                m = re.match(r"\s*([^\(]*)\(", post_match_string)
                if m:
                    transformed_string = cppvariable.as_string() + match_token.as_string() + post_match_string
                    debug.line("transform_return_cvariable_access", f"return value via function call transformed: {transformed_string}")
                    return transformed_string

                # Handle everything else: extract the assigned value and process...
                m = re.match(r"\s*([^,\);]*)(?:\s*[,\);])", post_match_string)
                assert m, f"Could not extract assigned value from: {post_match_string}"
                
                # Ignore GRIB_ return types as they will be processed later...
                if m.group(1).strip().startswith("GRIB_"):
                    debug.line("transform_return_cvariable_access", f"Ignoring (for now) GRIB_ return value [{m.group(1)}]")
                    return None

                # Finally: cast value to the return type...
                transformed_string = cppvariable.as_string() + match_token.as_string() + f" {ret}{{{m.group(1)}}}" + post_match_string[m.end(1):]
                debug.line("transform_return_cvariable_access", f"Casting to return type, transformed: {transformed_string}")
                return transformed_string

        return None

    # Customisation point for getting the correct container function call for the supplied action and data
    # For example: get_size could be .size() or .length()
    # Override as required
    def container_func_call_for(self, cpparg, action, data=""):
        if action in ["clear", "resize", "size"]:
            return f"{action}({data})"
        
        assert False, f"Unknown action [{action}]"


    # Special transforms for lengths that apply to buffers. The {ptr,length} C args are replaced by
    # a single C++ container arg, however we still need to deal with size-related code in the body
    # Note: The {ptr, length} and container arg indices are defined in the transforms object.
    #
    # For example:
    # cvariable match_token post_match_string   transformed_string
    # *len[0]   =           0;                  container.clear();
    # *len[0]   =           4;                  container.resize(4);
    # x         ==          *len;               x == container.size();
    # y         >           foo(*len);          y > foo(container.size());
    #
    # Note: Some code uses len[0] instead of *len, so we check for both...
    #
    def transform_len_cvariable_access(self, cvariable, match_token, post_match_string):
        mapping = self._transforms.funcsig_mapping_for(self._cfunction.name)
        if not mapping or not mapping.arg_indexes:
            return None
        
        buffer_arg_index = mapping.arg_indexes.cbuffer
        buffer_len_arg_index = mapping.arg_indexes.clength
        container_index = mapping.arg_indexes.cpp_container

        if not buffer_arg_index and not buffer_len_arg_index and not container_index:
            return None

        len_carg = mapping.cfuncsig.args[buffer_len_arg_index]
        assert len_carg, f"Len arg should not be None for c function: {mapping.cfuncsig.name}"

        if len_carg.name != cvariable.name:
            return None
        
        container_arg = mapping.cppfuncsig.args[container_index]
        assert container_arg, f"Container arg should not be None for C++ function: {mapping.cppfuncsig.name}"

        # Final check before applying transforms: is len_carg already referencing a transformed cpp container?
        # For example: *len = strlen(v); becomes *len = v.size() becomes *len = value.size() 
        #              *len is paired with v/value so the reference is invalid, and will be removed!
        #              See grib_accessor_class_bits.cc for an example of this
        m = re.match(r"\s*(\w*).(\w*)\(", post_match_string)
        if m and container_arg.name == m.group(1):
            if m.group(2) in ["size", "resize"]:
                transformed_line = f"// [removing invalid reference] " + cvariable.as_string() + match_token.as_string() + post_match_string
                debug.line("transform_len_cvariable_access", f"Removing invalid {cvariable.as_string()} reference to transformed container, Line:{transformed_line}")
                return transformed_line

        # Replace *len = N with CONTAINER.clear() if N=0, or CONTAINER.resize() the line if N is any other value
        if match_token.is_assignment:
            if container_arg.is_const():
                debug.line("transform_len_cvariable_access", f"Removed len assignment for const variable [{cvariable.as_string()}]")
                return f"// [length assignment removed - var is const] " + cvariable.as_string() + match_token.as_string() + post_match_string

            # Extract the assigned value
            m = re.match(r"\s*([^,\);]+)\s*[,\);]", post_match_string)
            assert m, f"Could not extract assigned value from: {post_match_string}"

            if m.group(1) == "0":
                container_func_call = self.container_func_call_for(container_arg, "clear")
                transformed_line = f"{container_arg.name}.{container_func_call};"
                debug.line("transform_len_cvariable_access", f"Replaced {cvariable.as_string()} = 0 with .{container_func_call} Line:{transformed_line}")
                return transformed_line
            else:
                container_func_call = self.container_func_call_for(container_arg, "resize", m.group(1))
                transformed_line = f"{container_arg.name}.{container_func_call};"
                debug.line("transform_len_cvariable_access", f"Replaced {cvariable.as_string()} = {m.group(1)} with .{container_func_call} Line:{transformed_line}")
                return transformed_line

        # Replace any other *len with CONTAINER.size()
        container_func_call = self.container_func_call_for(container_arg, "size")
        transformed_line = f"{container_arg.name}.{container_func_call}" + match_token.as_string() + post_match_string
        debug.line("transform_len_cvariable_access", f"Replaced {cvariable.as_string()} with {container_arg.name}.{container_func_call} Line:{transformed_line}")
        return transformed_line


    # Make sure all container variables have sensible assignments, comparisons etc
    def transform_container_cvariable_access(self, cvariable, match_token, post_match_string):
        cpp_container_arg = None
        for carg, cpparg in self._transforms.all_args.items():
            if carg.name == cvariable.name and cpparg and arg.is_container(cpparg):
                cpp_container_arg = cpparg
                break

        if not cpp_container_arg:
            return None

        if match_token.is_assignment:
            if cpp_container_arg.is_const():
                debug.line("transform_container_cvariable_access", f"Removed len assignment for const variable [{cvariable.as_string()}]")
                return f"// [length assignment removed - var is const] " + cvariable.as_string() + match_token.as_string() + post_match_string
            
            # Extract the assigned value
            m = re.match(r"\s*([^,\)\{};]+)\s*[,\)\{};]", post_match_string)
            assert m, f"Could not extract assigned value from: {post_match_string}"

            if m.group(1) == "NULL":
                debug.line("transform_container_cvariable_access", f"Replaced {cvariable.as_string()} = NULL with {{}}")
                return f"{cpp_container_arg.name} = {{}};"
            elif m.group(1) == "{":
                debug.line("transform_container_cvariable_access", f"Ignoring {cvariable.as_string()} braced initialiser [{post_match_string}]")
                return cpp_container_arg.name + match_token.as_string() + post_match_string
            elif m.group(1) == "0":
                debug.line("transform_container_cvariable_access", f"Replaced {cvariable.as_string()} = 0 with {{}}")
                post_match_string = re.sub(r"(\s*)0", r"\1{}", post_match_string)
                return cpp_container_arg.name + match_token.as_string() + post_match_string

        elif match_token.is_comparison: 
            # Extract the assigned value
            m = re.match(r"\s*([^,\)\{};]+)\s*[,\)\{};]", post_match_string)
            assert m, f"Could not extract assigned value from: {post_match_string}"

            if m.group(1) == "0":
                debug.line("transform_container_cvariable_access", f"Changed {cvariable.as_string()} == 0 comparison with .empty()")
                post_match_string = re.sub(r"\s*0", "", post_match_string)
                return f"{cpp_container_arg.name}.empty()" + post_match_string
            
            # TODO: Handle other comparisons?

        return None


    # Fallback if custom transforms don't match
    def default_transform_cvariable_access(self, cvariable, match_token, post_match_string):
        cppvariable = self.transform_if_cvariable(cvariable)
        if cppvariable:
            if not cppvariable.name:    # Marked for delete
                if match_token.is_assignment:
                    debug.line("default_transform_cvariable_access", f"Deleted [{cvariable.name}]")
                    return f"// [Deleted variable {cvariable.name}] " + cvariable.as_string() + match_token.as_string() + post_match_string
                debug.line("default_transform_cvariable_access", f"Removed [{cvariable.name}] for match [{match_token.value}]")
                if match_token.is_terminator:
                    return match_token.as_string() + post_match_string
                else:
                    return post_match_string

            return cppvariable.as_string() + match_token.as_string() + post_match_string
        
        else:
            # See if it a function name that hasn't been transformed (e.g. in an initializer list)
            cppvariable = self.transform_if_cfunction_name(cvariable)
            if cppvariable:
                return cppvariable.as_string() + match_token.as_string() + post_match_string
        
        return None

    # Takes the cvariable, match_value and post_match_string and return an updated string
    # representing the transform, or None if no transform available. For example:
    #
    # cvariable match_token post_match_string   transformed_string
    # *foo[0]   =           4;                  foo[4] = 4;
    # x         ==          value_count;        x = valueCount;
    # y         >           bar(*another_foo);  y > bar(anotherFoo);
    def transform_cvariable_access(self, cvariable, match_token, post_match_string):

        debug.line("transform_cvariable_access", f"[IN] cvariable=[{cvariable.as_string()}] match_token=[{match_token.value}] post_match_string=[{post_match_string}]")

        for transform_func in [
            self.custom_transform_cvariable_access,
            self.transform_return_cvariable_access,
            self.transform_len_cvariable_access,
            self.transform_container_cvariable_access,
            self.default_transform_cvariable_access,
        ]:
            transformed_string = transform_func(cvariable, match_token, post_match_string)
            
            if transformed_string:
                return transformed_string
        
        return None

    def update_cvariable_access(self, line, depth):
        assert depth<20, f"Unexpected recursion depth [{depth}]"

        # Regex groups:
        # 12     3   4
        # *my_foo[6] TOKEN
        #
        # TOKEN matches one of: 
        #   =               (group 5 - assignment)
        #   != == += >= etc (group 6) 
        #   , ) [ ] ;       (group 7 - terminator)
        #
        # Note:
        #   (?<!%) is to avoid matching e.g %ld in a printf
        #   (=(?!=)) matches exactly one "=" so we can distinguish "=" (group 5) from "==" (group 6)
        #   (?![<>\*&]) is added to ensure pointer/reference types and templates are not captured, e.g. in (grib_accessor*)self; or std::vector<double>
        #   Group 2 matches a character first to avoid matching numbers as variables
        m = re.search(r"([&\*])?\b((?<!%)[a-zA-Z][\w\.]*(?![<>\*&]))(\[\d+\])?\s*((=(?!=))|([!=<>&\|\+\-\*/%\?:]+)|([,\)\[\]\};]))", line)

        if m:
            if m.group(2) in ["vector", "string", "return", "break", "goto"]:
                debug.line("update_cvariable_access", f"IN  False match [{m.group(2)}] : {line}")
                remainder = self.update_cvariable_access(line[m.end():], depth+1)
                line = line[:m.end()] + remainder
                debug.line("update_cvariable_access", f"OUT False match [{m.group(2)}] : {line}")
            else:
                cvariable = variable.Variable(pointer=m.group(1), name=m.group(2), index=m.group(3))
                match_token = variable.MatchToken(m.group(4))
                debug.line("update_cvariable_access", f"IN  [{depth}][{cvariable.as_string()}][{match_token.value}]: {line}")

                # First process the remainder of the string (recursively), updating it along the way, so we can use it later...
                remainder = self.update_cvariable_access(line[m.end():], depth+1)

                transformed_remainder = self.transform_cvariable_access(cvariable, match_token, remainder)

                if transformed_remainder:
                    line = line[:m.start()] + transformed_remainder
                    debug.line("update_cvariable_access", f"OUT [{depth}][{cvariable.as_string()}][{match_token.value}]: {line}")
                else:
                    line = line[:m.end()] + remainder
                    debug.line("update_cvariable_access", f"OUT [{depth}][No transformed_remainder]: {line}")

        return line
        

    def update_cvariables(self, line):
        line = self.update_cvariable_access(line, 0)
        return line

    def process_ctype_cast(self, line, depth):
        assert depth<5, f"Unexpected recursion depth [{depth}]"

        m = re.search(r"\((\w+)\*+\)", line)

        if m:
            # First process the remainder of the string (recursively), updating it along the way, so we can use it later...
            remainder = self.process_ctype_cast(line[m.end():], depth+1)

            for ctype, cpptype in self._transforms.types.items():
                if ctype == m.group(1):
                    line = line[:m.start()] + f"({cpptype}*)" + remainder
                    debug.line("process_ctype_cast", f"[{depth}] Transformed cast [{m.group(0)}]: {line}")
                    return line

            # No transform - just apply remainder...
            line = line[:m.end()] + remainder

        return line
        
    def update_ctype_casts(self, line):
        line = self.process_ctype_cast(line, 0)
        return line

    # Transform any variables, definitions, etc with a "grib" type prefix...
    def apply_grib_api_transforms(self, line):
        line = grib_api_converter.convert_grib_api_definitions(line)
        return line

    # Regex will match one of:
    # 1. sizeof(x)
    # 2. sizeof(x)/sizeof(*x)
    # 3. sizeof(x)/sizeof(x)
    # 4. sizeof(x)/sizeof(x[0])
    #
    # Option 3 is required because converting to C++ vars can strip the *
    # Option 4 is required because some C code uses x[0] instead of *x
    #
    # Regex groups:
    #        1 2       3
    # sizeof(x)/sizeof(x[0])
    #
    def update_sizeof_calls(self, line):
        m = re.search(r"sizeof\((.*?)\)(\s*/\s*sizeof\(\s*(\*?\1(?:\[0\])?)\))?", line)
        if m:
            if m.group(2):
                # We assume sizeof(x)/sizeof(*x) or sizeof(x)/sizeof(x[0]) refers to a container with a size() member...
                line = re.sub(m.re, f"{m.group(1)}.size()", line)
                debug.line("update_sizeof_calls", f"sizeof(x)/sizeof(*x) transform [after ]: {line}")
            else:
                # See if sizeof(x) needs to be replaced by x.size()
                for cpparg in self._transforms.all_args.values():
                    if cpparg and cpparg.name == m.group(1) and arg.is_container(cpparg):
                        line = re.sub(m.re, f"{m.group(1)}.size()", line)
                        debug.line("update_sizeof_calls", f"sizeof(x) transform for container [after ]: {line}")
                        break

        return line


    def apply_get_set_substitutions(self, line):
        # [1] grib_[gs]et_TYPE[_array][_internal](...) -> unpackTYPE(...)
        # Note: This regex is complicated (!) by calls like grib_get_double(h, lonstr, &(lon[i]));
        #       The (?:&)?([\(\w\[\]\)]*)? section is added to match this and strip off the & (and it appears twice!)
        m = re.search(r"\bgrib_([gs]et)_(\w+?)(?:_array)?(?:_internal)?\(\s*(h\s*,)?\s*(\"?\w*\"?)\s*,?\s*(?:&)?([\(\w\[\]\)]*)?\s*,?\s*(?:&)?([\(\w\[\]\)]*)?\s*\)", line)
        if m:
            accessor_name = m.group(4)
            if accessor_name[0] == "\"":
                accessor_name = "AccessorName(" + accessor_name + ")"
            else:
                for k,v in self._transforms.all_args.items():
                    if v and v.name == accessor_name and v.type == "std::string":
                        accessor_name = "AccessorName(" + accessor_name + ")"

            if m.group(1) == "get":
                if m.group(2) == "size":
                    line = re.sub(m.re, f"getSizeHelper({accessor_name}, {m.group(5)})", line)
                else:
                    line = re.sub(m.re, f"unpack{m.group(2).capitalize()}Helper({accessor_name}, {m.group(5)})", line)
            else:
                line = re.sub(m.re, f"pack{m.group(2).capitalize()}Helper({accessor_name}, {m.group(5)})", line)

            debug.line("apply_get_set_substitutions", f"Result of substitution: {line}")

        return line
    
    # Convert any int return values where the function return type is GribStatus
    def convert_int_return_values(self, line):
        m = re.search(rf"\breturn\s+(\d+)\s*;", line)
        if m and self._cppfunction.return_type == "GribStatus":
            line = re.sub(f"{m.group(1)}", f"GribStatus{{{m.group(1)}}}", line)
            debug.line("convert_int_return_values", f"Updated int return value [{m.group(1)}] to GribStatus [after ]: {line}")

        return line
    
    # ======================================== UPDATE FUNCTIONS ========================================
    

    
    def update_cpp_line(self, line):

        # Note: These apply in order, be careful if re-arranging!
        update_functions = [
            # [1] Update C functions only
            self.update_grib_api_cfunctions,
            self.update_cfunction_names,
            self.custom_cfunction_updates,

            # [2] Update C variable declarations (inc. typedefs and function pointers)
            self.update_cvariable_declarations,
            self.update_cstruct_type_declarations,
            self.update_cfunction_pointers,

            # [3] Update C variable access (get/set)
            self.update_cstruct_variables,
            self.update_cvariables,

            # [4] All other updates...
            self.update_ctype_casts,
            self.apply_grib_api_transforms,
            self.update_sizeof_calls,
            self.apply_get_set_substitutions,
            self.convert_int_return_values,
        ]

        debug.line("update_cpp_line", f"--------------------------------------------------------------------------------")
        debug.line("update_cpp_line", f"[PROCESSING] {line}")

        # We need to run the skip_line() check after each function
        for update_func in update_functions:
            if self.skip_line(line):
                return line

            line = update_func(line)

        return line

    # This is the main entry point for updating the C++ function body...        
    # It can be called recursively to update split lines etc
    def update_cpp_body(self, lines):
        new_lines = []

        for line in lines:
            if not line:
                # Assume this is a blank line in the input, so retain it for clarity...
                new_lines.append(line)
                continue
            
            # Split comma-separated variable definitions into separate lines
            # We then call ourself recursively for each split line
            #m = re.match(r"^(\s*[^\*/,\(]+\s+)\**\w+\s*=?\s*(\w+)?,", line)
            m = re.match(r"^(\s*\w+\s+)\w+\s*=?\s*(\w+)?,", line)
            if m:
                debug.line("update_cpp_body", f"--------------------------------------------------------------------------------")
                debug.line("update_cpp_body", f"comma-separated vars [before]: {line}")
                line = line.replace(",", f";\n{m.group(1)}")
                split_lines = [l for l in line.split("\n")]
                for line in split_lines:
                    debug.line("update_cpp_body", f"comma-separated vars [after ]: {line}")
                split_lines = self.update_cpp_body(split_lines)
                new_lines.extend(split_lines)
                continue
            
            line = self.update_cpp_line(line)
            if line:
                new_lines.append(line)

        return new_lines

    # Takes the c function, converts the body to C++ code, and returns the lines
    def create_cpp_body(self):
        debug.line("create_cpp_body", f"\n============================== {self._cfunction.name} [IN]  ==============================\n")
        cpp_lines = self.update_cpp_body(self._cfunction.code)
        debug.line("create_cpp_body", f"\n============================== {self._cfunction.name} [OUT] ==============================\n")

        return cpp_lines