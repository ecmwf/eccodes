
import debug
import cppfunction

# Represents the conversion of a CCode class to C++

class CppCode:
    def __init__(self) -> None:
        self._global_declarations = []
        self._functions = []

    @property
    def global_declarations(self):
        return self._global_declarations

    def add_global_declaration(self, line):
        self._global_declarations.append(line)

    @property
    def functions(self):
        return self._functions

    def add_function(self, cppfunc):
        self._functions.append(cppfunc)

