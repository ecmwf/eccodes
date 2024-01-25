
import utils.debug as debug
import code_object.global_function as global_function

# Stores the code elements: functions and data members
class CodeElements:
    def __init__(self) -> None:
        self._functions = []
        self._constructor = None
        self._destructor = None
        self._member_functions = []
        self._virtual_member_functions = []
        self._data_members = []

    @property
    def global_function(self):
        return self._global_function

    @global_function.setter
    def global_function(self, global_function_instance):
        assert isinstance(global_function_instance, global_function.GlobalFunction), f"global_function_instance must be a GlobalFunction!"
        self._global_function = global_function_instance

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

    def add_data_member(self, data_member):
        self._data_members.append(data_member)
