
import debug
import code_object.cfunction as cfunction
import default.macro_details as macro_details

# Represents a coherent unit of C code that needs to be parsed together: usually a single C file
#
# Note: C code can be stored as the equivalent C++ class representation
class DefaultCCode:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._global_declarations = []
        self._functions = []

        # Class representation - START
        self._class_name = ""
        self._super_class_name = ""
        self._constructor = None
        self._destructor = None
        self._member_functions = []
        self._virtual_member_functions = []
        self._data_members = []
        # Class representation - END

        self._macro_details = macro_details.MacroDetails()

    @property
    def global_declarations(self):
        return self._global_declarations

    def add_global_declaration(self, node):
        self._global_declarations.append(node)

    @property
    def macro_details(self):
        return self._macro_details

    def add_macro_definition(self, def_node):
        self._macro_details.add_definition(def_node)

    def add_macro_instantiation(self, inst_node):
        self._macro_details.add_instantiation(inst_node)

    @property
    def functions(self):
        return self._functions

    # Class representation - START ##############################

    @property
    def class_name(self):
        return self._class_name

    @property
    def super_class_name(self):
        return self._super_class_name

    @property
    def constructor(self):
        return self._constructor

    @property
    def destructor(self):
        return self._destructor

    @property
    def member_functions(self):
        return self._member_functions

    @property
    def virtual_member_functions(self):
        return self._virtual_member_functions

    @property
    def data_members(self):
        return self._data_members

    def add_data_member(self, cmember):
        assert cmember not in self._data_members, f"member [{cmember.as_string()}] already defined for [{self._class_name}]"
        self._data_members.append(cmember)

    # Class representation - END   ##############################
        
    @property
    def cfilename(self):
        return self._cfilename

    @property
    def parent_cfilename(self):
        return ""

    # Add a function object from the funcsig and body (AST)
    # This version assumes it is a free function
    # Override to identify class member functions
    def add_function(self, cfuncsig, body):
        cfunc = cfunction.CFunction(cfuncsig, body)
        self._functions.append(cfunc)

    # Debug Support
    def dump_global_declarations(self):
        for entry in self.global_declarations:
            debug.line("dump_global_declarations", f"spelling=[{entry.spelling}] kind=[{entry.kind}]")

    def dump_data_members(self):
        for cmember in self._data_members:
            debug.line("dump_data_members", f"member=[{cmember.as_string()}]")

    def dump_functions(self):
        debug.line("dump_functions", f"Constructor=[{self._constructor.funcsig.as_string() if self._constructor else None}]")
        debug.line("dump_functions", f"Destructor=[{self._destructor.funcsig.as_string() if self._destructor else None}]")

        for entry in self._inherited_methods:
            debug.line("dump_functions", f"Inherited=[{entry.funcsig.as_string()}]")
        for entry in self._private_methods:
            debug.line("dump_functions", f"Private=[{entry.funcsig.as_string()}]")
        for entry in self._static_functions:
            debug.line("dump_functions", f"Static=[{entry.funcsig.as_string()}]")

    def dump_class_info(self):
        debug.line("dump_class_info", f"Class name=[{self._class_name}]")
        debug.line("dump_class_info", f"Super class name=[{self._super_class_name}]")

    def dump(self):
        self.dump_class_info()
        self.dump_data_members()
        self.dump_global_declarations()
        self.dump_functions()