
import utils.debug as debug
from code_object.funcsig import FuncSig

# Stores info about the code, such as class name, includes etc
#
# NOTE: Any data stored here will be part of the final CppCode object
class CodeInfo:
    def __init__(self, file_name, class_name, super_class_name="") -> None:
        self._file_name = file_name
        self._class_name = class_name
        self._super_class_name = super_class_name
        self._namespaces = []
        self._forward_declarations = []
        self._base_function_names = [] # i.e. for using ClassName::funcName entries
        self._header_includes = []
        self._source_includes = []
        self._function_calls = [] # List all function calls made whilst converting (for post-processing)
        self._non_const_member_function_names = [] # default to const unless told otherwise!

    @property
    def file_name(self):
        return self._file_name

    @property
    def class_name(self):
        return self._class_name

    @property
    def super_class_name(self):
        return self._super_class_name

    @property
    def namespaces(self):
        return self._namespaces

    @property
    def forward_declarations(self):
        return self._forward_declarations

    @property
    def base_function_names(self):
        return self._base_function_names 

    @property
    def header_includes(self):
        return self._header_includes

    @property
    def source_includes(self):
        return self._source_includes

    @property
    def function_calls(self):
        return self._function_calls

    @property
    def non_const_member_function_names(self):
        return self._non_const_member_function_names

    def add_namespace(self, entry):
        self._namespaces.append(entry)

    def add_forward_declaration(self, entry):
        self._forward_declarations.append(entry)

    def add_base_function_name(self, funcname):
        if funcname not in self._base_function_names:
            self._base_function_names.append(funcname)

    def add_header_include(self, entry):
        self._header_includes.append(entry)

    def add_source_include(self, entry):
        self._source_includes.append(entry)

    def add_function_call_entry(self, entry):
        assert isinstance(entry, FuncSig), f"add_function_call_entry requires FuncSig, not [{entry}]"

        for func_call in self._function_calls:
            if entry.name == func_call.name:
                return # Duplicate

        self._function_calls.append(entry)

    def add_non_const_member_function_name(self, name):
        if name not in self._non_const_member_function_names:
            self._non_const_member_function_names.append(name)
