import debug
from code_object.declaration_specifier import DeclSpec
import code_object.code_interface as code_interface

# Represent an initialiser list
# Entries consist of a list of (list of) Args, e.g. {{arg1, arg2}, {arg3, arg4}}
# {arg1, arg2} is an entry
#
# NOTE: This class is recursive, you can add an InitList entry
class InitList(code_interface.CodeInterface):
    # Note: decl_spec can be a string or an instance of class DeclSpec
    def __init__(self, decl_spec) -> None:
        if isinstance(decl_spec, DeclSpec):
            self._decl_spec = DeclSpec.from_instance(decl_spec)
        else:
            self._decl_spec = DeclSpec.from_decl_specifier_seq(decl_spec)
        self._entries = []

    # Use decl_spec to get storage_class etc...
    @property
    def decl_spec(self):
        return self._decl_spec

    @property
    def entries(self):
        return self._entries

    # Each entry should be a subclass of code_interface
    def add_entry(self, entry):
        self._entries.append(entry)

    def as_lines(self):
        lines = []

        if len(self._entries) == 0:
            return lines

        if isinstance(self._entries[0], InitList):
            line = "{\n" + ',\n'.join(e.as_string() for e in self._entries) + "\n}"
        else:
            line = f"{{{', '.join(e.as_string() for e in self._entries)}}}"
    
        lines.append(line)

        return lines
