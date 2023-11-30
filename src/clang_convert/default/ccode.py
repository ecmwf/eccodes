
import debug
import cfunction

# Represents a coherent unit of C code that needs to be parsed together: usually a single C file

class CCode:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._global_declarations = []
        self._functions = []

    @property
    def global_declarations(self):
        return self._global_declarations

    def add_global_declaration(self, node):
        self._global_declarations.append(node)

    @property
    def functions(self):
        return self._functions
    
    @property
    def cfilename(self):
        return self._cfilename

    @property
    def parent_cfilename(self):
        return ""

    # Create a function object from the funcsig and body (AST)
    def add_function(self, cfuncsig, body):
        cfunc = cfunction.CFunction(cfuncsig, body)
        self._functions.append(cfunc)

    # Debug Support
    def dump_global_declarations(self):
        for entry in self.global_declarations:
            debug.line("dump_global_declarations", f"spelling=[{entry.spelling}] kind=[{entry.kind}]")

    def dump_functions(self):
        for entry in self.functions:
            debug.line("dump_functions", f"cfuncsig=[{entry.funcsig.as_string()}]")

    def dump(self):
        self.dump_global_declarations()
        self.dump_functions()