
from method_conv import *
import inherited_method

# Specialisation of MethodConverter for AccessorData Methods
class InheritedMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return inherited_method.InheritedMethod(cppfuncsig)
    
    def to_cpp_name(self, cfuncsig):
        name = member_function_transforms.transformed_name(cfuncsig.name)
        if name:
            return name
        else:
            debug.line("to_cpp_name", f"Calling super...")
            return super().to_cpp_name(cfuncsig)

    def to_cpp_return_type(self, cfuncsig):
        ret = member_function_transforms.return_type(cfuncsig.name)
        if ret:
            return ret
        else:
            return super().to_cpp_return_type(cfuncsig)

    def to_cpp_args(self, cfuncsig):
        cppargs = member_function_transforms.transformed_args(cfuncsig.name)

        if cppargs:
            return cppargs
        else:        
            return super().to_cpp_args(cfuncsig)
    
    # Special-handling for lengths. The len C param is removed because we use containers, however we need to
    # deal with size-related code
    def process_len_arg(self, line):
        if len(self._cfunction.args) < 3:
            return line
        
        len_carg = self._cfunction.args[2]
        if len_carg.name != "len":
            return line
        
        cppargs = member_function_transforms.transformed_args(self._cfunction.name)
        if not cppargs:
            return license

        len_cpparg = cppargs[1]

        # Note: Some code uses len[0] instead of *len, so we check for both...

        # Replace *len = N with CONTAINER.clear() if N=0, or CONTAINER.resize() the line if N is any other value
        m = re.search(rf"\*?\b{len_carg.name}\b(\[0\])?\s*=\s*(\w+).*?;", line)
        if m:
            if len_cpparg.is_const():
                line = re.sub(rf"^(\s*)", rf"// [length assignment removed - var is const] \1", line)
                debug.line("process_len_arg", f"Removed len assignment for const variable [after]: {line}")
            elif m.group(2) == "0":
                line = re.sub(rf"{re.escape(m.group(0))}", rf"{len_cpparg.name}.clear();", line)
                debug.line("process_len_arg", f"Replaced *len = 0 with .clear() [after]: {line}")
            else:
                line = re.sub(rf"{re.escape(m.group(0))}", rf"{len_cpparg.name}.resize({m.group(2)});", line)
                debug.line("process_len_arg", f"Replaced *len = N with .resize(N) [after]: {line}")

        # Replace *len <=> N with CONTAINER.size() <=> N
        m = re.search(rf"\*?\b{len_carg.name}\b(\[0\])?\s*([<>!=]=?)\s*(\w+)", line)
        if m:
            line = re.sub(rf"{re.escape(m.group(0))}", rf"{len_cpparg.name}.size() {m.group(2)} {m.group(3)}", line)
            debug.line("process_len_arg", f"Replaced *len <=> N with .size() <=> N [after]: {line}")

        # Replace any other *len with CONTAINER.size() <=> N
        m = re.search(rf"\*?\b{len_carg.name}\b(\[0\])?", line)
        if m:
            line = re.sub(rf"{re.escape(m.group(0))}", rf"{len_cpparg.name}.size()", line)
            debug.line("process_len_arg", f"Replaced *len <=> N with .size() <=> N [after]: {line}")

        return line

    # Override this to provide any initial conversions before the main update_line runs
    def process_variables_initial_pass(self, line):

        line = self.process_len_arg(line)

        return super().process_variables_initial_pass(line)

