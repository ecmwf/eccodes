import utils.debug as debug
import arg_conv
import func
from jinja2 import Environment, FileSystemLoader, StrictUndefined

# Represents the converted C++ code for an Accessor
class AccessorData:
    def __init__(self, name, super, factory_name):
        self._name = name
        self._super = super
        self._factory_name = factory_name
        self._members = []
        self._header_includes = []
        self._source_includes = []
        self._forward_declarations = []
        self._global_function = None
        self._inherited_methods = []
        self._private_template_methods = []
        self._private_methods = []
        self._static_functions = []
        self._constructor = None
        self._destructor = None
        self._inherited_method_using_list = []

    @property
    def name(self):
        return self._name

    @property
    def camel_case_name(self):
        debug.line("camel_case_name", f"name = {self._name}")
        return arg_conv.transform_variable_name(self._name)

    @property
    def super(self):
        return self._super

    @property
    def factory_name(self):
        return self._factory_name

    @property
    def class_type(self):
        return "accessor"

    @property 
    def namespaces(self):
        return ["eccodes", self.class_type]

    @property 
    def nested_namespaces(self):
        return "::".join(self.namespaces)

    @property
    def members(self):
        return self._members

    @property
    def header_includes(self):
        return self._header_includes

    @property
    def source_includes(self):
        return self._source_includes

    @property
    def forward_declarations(self):
        return self._forward_declarations

    @property
    def global_function(self):
        return self._global_function

    @property
    def inherited_methods(self):
        return self._inherited_methods

    @property
    def private_template_methods(self):
        return self._private_template_methods

    @property
    def private_methods(self):
        return self._private_methods

    @property
    def static_functions(self):
        return self._static_functions

    @property
    def constructor(self):
        return self._constructor

    @property
    def destructor(self):
        return self._destructor
    
    @property
    def inherited_method_using_list(self):
        return self._inherited_method_using_list

    @global_function.setter
    def global_function(self, value):
        if value:
            self._global_function = value

    def add_header_include(self, entry):
        self._header_includes.append(entry)

    def add_source_include(self, entry):
        self._source_includes.append(entry)

    def add_forward_declaration(self, entry):
        if not entry in self._forward_declarations:
            self._forward_declarations.append(entry)

    def add_member(self, member):
        self._members.append(member)

    def add_inherited_method(self, func):
        self._inherited_methods.append(func)
        for entry in func.using:
            if entry not in self._inherited_method_using_list:
                self._inherited_method_using_list.append(entry)

    def add_private_method(self, func):
        # Put templates in a separate list, so they can be created first
        if func.template:
            self._private_template_methods.append(func)
        else:
            self._private_methods.append(func)

    def add_static_function(self, func):
        self._static_functions.append(func)
