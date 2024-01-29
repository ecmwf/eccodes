
import utils.debug as debug

# Stores C to C++ mappings to be used by conversion_data
class CodeMappings:
    def __init__(self, ) -> None:
        self._funcbody_type_mappings = {}
        self._funcbody_arg_mappings = {}
        self._funcsig_arg_mappings = {}
        self._funcsig_type_mappings = {}
        self._funcsig_buffer_mappings = []  # Don't expect more than one entry, but just in case!
        self._data_member_mappings = {}
        self._funcsig_mappings = []
        self._funcsig_pointer_mappings = []
        self._member_funcsig_mappings = []
        self._virtual_member_funcsig_mappings = []
        self._literal_mappings = {}
        self._class_pointer_names = [] # self, this, etc
        self._container_types = [] # std::vector etc

    # Note: Types are stored as declaration_specifiers objects for flexibility, but usually we just match type and pointer!
    @property
    def funcbody_type_mappings(self):
        return self._funcbody_type_mappings

    @property
    def funcbody_arg_mappings(self):
        return self._funcbody_arg_mappings

    @property
    def funcsig_arg_mappings(self):
        return self._funcsig_arg_mappings

    @property
    def funcsig_type_mappings(self):
        return self._funcsig_type_mappings
    
    @property
    def funcsig_buffer_mappings(self):
        return self._funcsig_buffer_mappings

    @property
    def data_member_mappings(self):
        return self._data_member_mappings

    @property
    def all_funcsig_mappings(self):
        return self._funcsig_mappings + self._member_funcsig_mappings + self._virtual_member_funcsig_mappings

    @property
    def funcsig_mappings(self):
        return self._funcsig_mappings

    @property
    def funcsig_pointer_mappings(self):
        return self._funcsig_pointer_mappings

    @property
    def member_funcsig_mappings(self):
        return self._member_funcsig_mappings

    @property
    def virtual_member_funcsig_mappings(self):
        return self._virtual_member_funcsig_mappings

    @property
    def literal_mappings(self):
        return self._literal_mappings
    
    @property
    def class_pointer_names(self):
        return self._class_pointer_names
    
    @property
    def container_types(self):
        return self._container_types
