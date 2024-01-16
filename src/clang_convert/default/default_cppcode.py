
import debug
import os
import code_object.cppfunction as cppfunction
import code_object.code_objects as code_objects

# Represents the C++ conversion of a CCode instance
# Stored as CodeInterface objects
class DefaultCppCode:
    def __init__(self, filename, class_name="", super_class_name="") -> None:
        self._global_declarations = code_objects.CodeObjects()
        self._functions = []
        self._filename = filename
        self._class_name = class_name
        self._super_class_name = super_class_name
        self._constructor = None
        self._destructor = None
        self._member_functions = []
        self._virtual_member_functions = []
        self._data_members = []
        self._header_file_includes = []
        self._source_file_includes = []

    @property
    def filename(self):
        return self._filename

    @property
    def file_stem(self):
        return os.path.splitext(os.path.basename(self._filename))[0]

    @property
    def class_name(self):
        return self._class_name

    @property
    def super_class_name(self):
        return self._super_class_name

    @property 
    def nested_namespaces(self):
        return "NAMESPACE1::NAMESPACE2"

    @property 
    def forward_declarations(self):
        return []

    @property
    def global_declarations(self):
        return self._global_declarations

    def add_global_declaration(self, global_decl_ccode_objects):
        self._global_declarations = global_decl_ccode_objects

    @property
    def functions(self):
        return self._functions

    def add_function(self, cppfunc):
        self._functions.append(cppfunc)

    @property
    def constructor(self):
        return self._constructor

    def add_constructor(self, constructor):
        self._constructor = constructor

    @property
    def destructor(self):
        return self._destructor

    def add_destructor(self, destructor):
        self._destructor = destructor

    @property
    def member_functions(self):
        return self._member_functions

    def add_member_function(self, member_function):
        self._member_functions.append(member_function)

    @property
    def template_member_functions(self):
        return self._member_functions

    @property
    def virtual_member_functions(self):
        return self._virtual_member_functions

    def add_virtual_member_function(self, virtual_member_function):
        self._virtual_member_functions.append(virtual_member_function)

    @property
    def virtual_member_functions_using_list(self):
        return []

    @property
    def data_members(self):
        return self._data_members

    def add_data_member_function(self, data_member):
        self._data_members.append(data_member)

    @property
    def header_file_includes(self):
        return self._header_file_includes

    def add_header_file_include(self, file):
        self._header_file_includes.append(file)

    @property
    def source_file_includes(self):
        return self._source_file_includes

    def add_source_file_include(self, file):
        self._source_file_includes.append(file)
