
import utils.debug as debug

# Stores C to C++ mappings to be used by conversion_data
class CodeMappings:
    def __init__(self, ) -> None:
        self._type_mappings = {}
        self._arg_mappings = {}
        self._function_arg_mappings = {}
        self._funcsig_mappings = []
        self._funcsig_pointer_mappings = []
        self._member_funcsig_mappings = []
        self._virtual_member_funcsig_mappings = []

    # Note: Types are stored as declaration_specifiers objects for flexibility, but usually we just match type and pointer!
    @property
    def type_mappings(self):
        return self._type_mappings

    @property
    def arg_mappings(self):
        return self._arg_mappings

    @property
    def function_arg_mappings(self):
        return self._function_arg_mappings

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
