
from method_conv import *
import inherited_method

# Specialisation of MethodConverter for AccessorData Methods
class InheritedMethodConverter(MethodConverter):
    def __init__(self):
        super().__init__()

    def create_cpp_function(self, cppfuncsig):
        return inherited_method.InheritedMethod(cppfuncsig)
    
    # Special-handling for lengths. The len C param is removed because we use containers, however we need to
    # deal with size-related code
    def process_len_arg(self, line):
        buffer_arg_index, buffer_len_arg_index = self._transforms.buffer_and_len_arg_indexes(self._cfunction.name)

        if not buffer_arg_index and not buffer_len_arg_index:
            return line

        len_carg = self._cfunction.args[buffer_len_arg_index]
        if not len_carg.name:
            return line
        
        cppfuncsig = self._transforms.cppfuncsig_for(self._cfunction.func_sig)
        if not cppfuncsig:
            return line
        
        cppargs = cppfuncsig.args
        if not cppargs:
            return line

        len_cpparg = cppargs[1] # Need to check this!

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

