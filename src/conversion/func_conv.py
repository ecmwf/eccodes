# Convert C function to C++
import debug
import func
import funcsig
import arg
import arg_conv
import c_def_conv
import c_subs
import re

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
        cppfuncsig = self._transforms.cppfuncsig_for(self._cfunction.func_sig)

        # Store the C to C++ function arg transforms
        for index, carg in enumerate(self._cfunction.func_sig.args):
            cpparg = cppfuncsig.args[index]
            self._transforms.add_local_args(carg, cpparg)

        self._cppfunction = self.create_cpp_function(cppfuncsig)
        for line in self.create_cpp_body():
            self._cppfunction.add_line(line)
        
        return self._cppfunction

    def create_cpp_function(self, cppfuncsig):
        assert False, "[WARNING] Unexpected call to FunctionConverter instance - consider overriding!"
        return func.Function(cppfuncsig)

    def skip_line(self, line):
        if not line:
            debug.line("skip_line", f"[Empty]: {line}")
            return True

        if re.match(r"^\s*//", line):
            debug.line("skip_line", f"[C++ Comment]: {line}")
            return True

        m = re.match(r"^\s*(/\*)?.*?(\*/)?$", line)

        if m and m.group(1):
            if m.group(2):
                self._in_comment_block = False
                debug.line("skip_line", f"[C Comment]: {line}")
            else:
                debug.line("skip_line", f"[C Comment Start]: {line}")
                self._in_comment_block = True
            return True
            
        if m and m.group(2):
            self._in_comment_block = False
            debug.line("skip_line", f"[C Comment End  ]: {line}")
            return True
        
        if self._in_comment_block:
            debug.line("skip_line", f"[C Comment Block]: {line}")
            return True

        return False

    # Override this to provide any initial conversions before the main update_cpp_line runs
    def process_variables_initial_pass(self, line):
        return line

    # Find type declarations and store in the transforms
    def process_type_declarations(self, line):

        # struct declaration [1]: [typedef] struct S [S]
        m = re.match(r"^(typedef)?\s*struct\s+(\w+)\s*(\w*)?(;)?$", line)
        if m:
            if m.group(1) and m.group(4):
                line = "// [Removed struct typedef] " + line
            else:
                ctype = m.group(2)
                cpptype = arg_conv.transform_type_name(ctype)

                self._transforms.add_to_types(ctype, cpptype)

                line = re.sub(m.re, f"struct {cpptype}", line)
                debug.line("process_type_declarations", f"Added struct type transform: {ctype} -> {cpptype} [after ]: {line}")

                carg = arg.Arg("struct", ctype)
                cpparg = arg.Arg("struct", cpptype)
                self._transforms.add_local_args(carg, cpparg)
                debug.line("process_type_declarations", f"Adding struct to local arg map: {carg.type} {carg.name} -> {cpparg.type} {cpparg.name} [after ]: {line}")
        
        # struct declaration [2]: } S
        m = re.match(r"^}\s*(\w+)", line)
        if m and m.group(1) not in ["else"]:
            # We'll assume this definition is covered by [1]
            line = re.sub(m.re, "}", line)
            debug.line("process_type_declarations", f"Removed extraneous struct definition: {m.group(1)} [after ]: {line}")

        return line

    # Update any well know return values
    def process_return_variables(self, line):
        ret = "GribStatus"
        if self._cppfunction.return_type == ret:
            for ret_var in ["err", "ret"]:
                # [1] Assigning to function result - we assume the function returns the correct type!
                m = re.search(rf"\bint\s+{ret_var}\s*=\s*(.*)\(", line)
                if m:
                    line = re.sub(m.re, rf"{ret} {ret_var} = {m.group(1)}(", line)
                    debug.line("process_return_variables", f"return value assigned via function [after ]: {line}")

                m = re.search(rf"\bint\b(\s+{ret_var}\s+=\s*)(\d+)[,;]", line)
                if m:
                    line = re.sub(m.re, rf"{ret}\1{ret}{{\2}};", line)
                    debug.line("process_return_variables", f"return value assigned to value [after ]: {line}")

                m = re.search(rf"(\(\s*{ret_var}\s*)\)", line)
                if m:
                    line = re.sub(m.re, rf"\1 != {ret}::SUCCESS)", line)
                    debug.line("process_return_variables", f"return value comparison updated [after ]: {line}")

        return line
                
    # Find variable declarations with assignments, e.g. char* buf = "Data"; and then:
    # 1. Update the type if required / delete the line if no longer valid
    # 2. Store in the arg_map for future reference
    def process_variable_declarations(self, line):

        # Note: "return x;" looks like a variable declaration, so we explicitly exclude it
        #       static and const are ignored
        prefix = r"^\s*(?!return)(?:static)?(?:const)?"
        m = re.match(rf"{prefix}\s*((\w+\*?\*?)\s+(\w+)\s*(\[\d*\])?)\s*[=;]", line)

        if m:
            carg = arg.Arg.from_string(m.group(1))
            debug.line("process_variable_declarations", f"MATCH: {m.group(0)} : {line}")

        if m and carg:
            arg_converter = arg_conv.ArgConverter(carg)
            cpparg = arg_converter.to_cpp_arg(self._transforms.types)

            if not cpparg:
                debug.line("process_variable_declarations", f"Found var declaration to delete: {carg.type} {carg.name}")
                self._transforms.add_local_args(carg, None)
                debug.line("process_variable_declarations", f"--> deleting: {line}")
                return ""
            else:
                debug.line("process_variable_declarations", f"Found var declaration to store: {carg.type} {carg.name} -> {cpparg.type} {cpparg.name}")
                self._transforms.add_local_args(carg, cpparg)
                if carg.type != cpparg.type or carg.name != cpparg.name:
                    line = re.sub(rf"{re.escape(m.group(1))}", f"{cpparg.as_declaration()}", line)
                    debug.line("process_variable_declarations", f"Transformed line: {line}")

        return line

    # Remove any variables that have been marked for deletion
    def process_deleted_variables(self, line):
        # Find any deleted variables that are being assigned to, and delete the line
        m = re.match(r"^\s*\**(\w+)\s*=", line)
        if m:
            debug.line("process_deleted_variables", f"Found var assignment: var={m.group(1)}: {line}")
            for carg, cpparg in self._transforms.all_args.items():
                if carg.name == m.group(1) and not cpparg:
                    debug.line("process_deleted_variables", f"Var assignment marked for delete, var={m.group(1)} - deleting: {line}")
                    line = f"// [{m.group(1)} removed] " + line
                    return line
        
        # Remove any deleted vars that remain (i.e. as an argument to a function call)
        for carg, cpparg in self._transforms.all_args.items():
            if not cpparg and re.match(rf"^.*\b{carg.name}\b\s*,*", line):
                line = re.sub(rf"[&\*]?\b{carg.name}(->)?\b\s*,*", "", line)
                debug.line("process_deleted_variables", f"Removing arg={carg.name} [after ]: {line}")

        return line

    def apply_variable_transforms(self, line):
        # Assume sizeof(x)/sizeof(*x) or sizeof(x)/sizeof(x[0]) refers to a container with a size() member...
        m = re.search(r"\bsizeof\((.*?)\)\s*/\s*sizeof\(\s*(?:\*)?\1(?:\[0\])?\s*\)", line)
        if m:
            line = re.sub(m.re, f"{m.group(1)}.size()", line)
            debug.line("apply_variable_transforms", f"sizeof transform [after ]: {line}")

        # See if sizeof(x) needs to be replaced by x.size()
        m = re.search(r"\bsizeof\((.*?)\)", line)
        if m:
            for _, cpparg in self._transforms.all_args.items():
                if cpparg and cpparg.name == m.group(1):
                    if arg.is_container(cpparg):
                        line = re.sub(m.re, f"{m.group(1)}.size()", line)
                        debug.line("apply_variable_transforms", f"sizeof(x) transform for container [after ]: {line}")
                        break

        return line

    # Special handling for function pointers: [typedef] RET (*PFUNC)(ARG1, ARG2, ..., ARGN);
    def process_function_pointers(self, line):

        m = re.match(r"^(?:typedef)\s*(\w+\**)\s*\(\s*\*(\s*\w+)\s*\)\s*\((.*)\)", line)
        if m:
            carg = arg.Arg(m.group(1), m.group(2))
            arg_converter = arg_conv.ArgConverter(carg)
            cpparg = arg_converter.to_cpp_arg(self._transforms.types)

            # Assume functions returning int will now return GribStatus
            if cpparg.type == "int":
                cpparg.type = "GribStatus"

            debug.line("process_function_pointers", f"Adding var to local arg map: {carg.type} {carg.name} -> {cpparg.type} {cpparg.name} [after ]: {line}")
            self._transforms.add_local_args(carg, cpparg)

            # Parse the function arg types
            cpp_arg_types = []
            for arg_type in [a.strip() for a in m.group(3).split(",")]:
                arg_converter = arg_conv.ArgConverter(arg.Arg(arg_type, "Dummy"))
                cpp_sig_arg = arg_converter.to_cpp_func_sig_arg()
                if cpp_sig_arg:
                    cpp_arg_types.append(cpp_sig_arg.type)
            
            # Apply the transform
            line = re.sub(rf"(\w+\**)\s*\(\s*\*(\s*\w+)\s*\)\s*\((.*)\)", f"{cpparg.type}(*{cpparg.name})({','.join([a for a in cpp_arg_types])})", line)
            debug.line("process_function_pointers", f"Transformed line: {line}")

        return line
    
    def process_remaining_cargs(self, line):

        # Update any C arg that remain (i.e. as an argument to a function call)
        # This will also remove any unnecessary pointers/refs
        # Note: We ignore anything in quotes!
        for carg, cpparg in self._transforms.all_args.items():
            debug.line("process_remaining_cargs", f"carg={arg.arg_string(carg)} cpparg={arg.arg_string(cpparg)}")

            if not cpparg:
                continue

            # Note: We assume *var and &var should be replaced with var
            m = re.search(rf"(?<!\")[&\*]?\b{carg.name}\b(\s*,*)(?!\")", line)
               
            if m and m.group(0) != cpparg.name:
                line = re.sub(rf"{re.escape(m.group(0))}", rf"{cpparg.name}{m.group(1)}", line)
                debug.line("process_remaining_cargs", f"Substituted \"{m.group(0)}\" with \"{cpparg.name}{m.group(1)}\" [after ]: {line}")

        return line

    # Update any references to global args
    def process_global_cargs(self, line):
        # Update any global C args used (i.e. as an argument to a function call)
        # This will also remove any unnecessary pointers/refs
        # Note: We ignore anything in quotes!
        for carg, cpparg in self._transforms.all_args.items():
            if not cpparg:
                continue

            # Note: We assume *var and &var should be replaced with var
            m = re.search(rf"(?<!\")[&\*]?\b{carg.name}\b(\s*,*)(?!\")", line)

            if m and m.group(0) != cpparg.name:
                line = re.sub(rf"{re.escape(m.group(0))}", rf"{cpparg.name}{m.group(1)}", line)
                debug.line("process_global_cargs", f"Substituted \"{m.group(0)}\" with \"{cpparg.name}\"{m.group(1)}\" [after ]: {line}")

        return line

    def convert_grib_values(self, line):
        for k, v in c_def_conv.GribStatusConverter.items():
            line, count = re.subn(rf"{k}", rf"{v}", line)
            if count:
                debug.line("convert_grib_values", f"Replaced {k} with {v} [after ]: {line}")
        
        for k, v in c_def_conv.GribTypeConverter.items():
            line, count = re.subn(rf"{k}", rf"{v}", line)
            if count:
                debug.line("convert_grib_values", f"Replaced {k} with {v} [after ]: {line}")
        
        for k, v in c_def_conv.GribAccessorFlagConverter.items():
            line, count = re.subn(rf"{k}", rf"toInt({v})", line)
            if count:
                debug.line("convert_grib_values", f"Replaced {k} with {v} [after ]: {line}")
        
        return line
        
    def convert_grib_utils(self, line):
        for util in c_def_conv.GribUtilFuncs:
            m = re.search(rf"\b{util}\b", line)
            if m:
                cpp_util = transform_function_name(util)
                line = re.sub(m.re, f"{cpp_util}", line)
                debug.line("convert_grib_utils", f"Replaced {util} with {cpp_util} [after ]: {line}")

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

    def apply_function_transforms(self, line):
        line = c_subs.apply_all_substitutions(line)

        # Static function substitutions
        m = re.search(rf"(?<!\")(&)?\b(\w+)\b(?!\")", line)
        if m:
            for cfuncsig, cppfuncsig in self._transforms.static_funcsigs.items():
                if m.group(2) == cfuncsig.name:
                    prefix = m.group(1) if m.group(1) is not None else ""
                    line = re.sub(m.re, rf"{prefix}{cppfuncsig.name}", line)
                    debug.line("apply_function_transforms", f"Updating static function {m.group(0)} [after ]: {line}")

        return line

    # Make sure all container variables have sensible assignments, comparisons etc after any transformations
    def validate_container_variables(self, line):
        for k, v in self._transforms.all_args.items():
            if not v or not arg.is_container(v):
                continue

            # [1] Assignments
            m = re.search(rf"\b{v.name}\s*=\s*(\"?\w+\"?).*?;", line)
            if m:
                if m.group(1) == "NULL":
                    line = line.replace("NULL", "{}")
                    debug.line("validate_container_variables", f"Updated NULL assigned value [after ]: {line}")
                elif m.group(1) == "0":
                    # Replace CONTAINER = 0 with CONTAINER.clear()
                    line = re.sub(m.re, f"{v.name}.clear();", line)
                    debug.line("validate_container_variables", f"Changed = 0 assignment to .clear() for container [after ]: {line}")

            # [2] Empty comparisons (== 0)
            m = re.search(rf"\b{v.name}(\s*==\s*0)\b", line)
            if m:
                line = re.sub(m.re, f"{v.name}.empty()", line)
                debug.line("validate_container_variables", f"Changed == 0 comparison to .empty() for container [after ]: {line}")

        return line
    
    # Anything else that's needed
    def apply_final_checks(self, line):
        m = re.search(rf"\breturn\s+(\d+)\s*;", line)
        if m and self._cppfunction.return_type == "GribStatus":
            line = re.sub(f"{m.group(1)}", f"GribStatus{{{m.group(1)}}}", line)
            debug.line("apply_final_checks", f"Updated return value to GribStatus [after ]: {line}")

        return line
    
    # Override this to provide any function transforms specific to a class
    def special_function_transforms(self, line):
        return line

    def update_cpp_line(self, line):

        # Note: These apply in order, be careful if re-arranging!
        update_functions = [
            # [1] function updates that expect the original C variable names
            self.convert_grib_utils,
            self.apply_function_transforms,
            self.special_function_transforms,

            # [2] The remaining updates must work with C variables that may have been renamed to C++
            self.process_variables_initial_pass,
            self.process_type_declarations,
            self.process_return_variables,
            self.process_variable_declarations,
            self.process_deleted_variables,
            self.apply_variable_transforms,
            self.process_function_pointers,
            self.process_remaining_cargs,
            self.process_global_cargs,
            self.convert_grib_values,
            self.apply_get_set_substitutions,
            self.validate_container_variables,
            self.apply_final_checks,
        ]

        debug.line("update_cpp_line", f"--------------------------------------------------------------------------------")
        debug.line("update_cpp_line", f"PROCESSING: [ {line} ]")

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
            m = re.match(r"^(\s*\w+\s+)\w+\s*=?\s*(\w+)?,", line)
            if m:
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
