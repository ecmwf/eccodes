
import utils.debug as debug
from code_object.code_interface import as_debug_string

# Stores info about the code, such as class name, includes etc
class CodeInfo:
    def __init__(self, file_name, class_name, super_class_name="") -> None:
        self._file_name = file_name
        self._class_name = class_name
        self._super_class_name = super_class_name
        self._namespaces = []
        self._forward_declarations = []
        self._header_includes = []
        self._source_includes = []

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
    def header_includes(self):
        return self._header_includes

    @property
    def source_includes(self):
        return self._source_includes

    def add_namespace(self, entry):
        self._namespaces.append(entry)

    def add_forward_declaration(self, entry):
        self._forward_declarations.append(entry)

    def add_header_include(self, entry):
        self._header_includes.append(entry)

    def add_source_include(self, entry):
        self._source_includes.append(entry)
