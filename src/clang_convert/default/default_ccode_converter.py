
import utils.debug as debug
import cpp_code.cppcode as cppcode
import cpp_code.code_info as code_info
import cpp_code.code_elements as code_elements
import code_object_converter.supporting.conversion_data as conversion_data 
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.data_member as data_member
from default.default_conversion_validation import DefaultConversionValidation
import code_object.member_function as member_function

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class DefaultCCodeConverter:
    def __init__(self, ccode_instance) -> None:
        self._ccode = ccode_instance

    def convert(self):
        self.setup_conversion_objects()
        self.add_includes()
        self.convert_global_function()
        self.convert_data_members()

        # Now set the conversion data state to local for the rest of the conversion
        self._conversion_data.set_local_state()

        self.convert_functions()
        self.convert_constructor_function()
        self.convert_destructor_function()
        self.convert_member_functions()
        self.convert_virtual_member_functions()

        return cppcode.CppCode(self._code_info, self._code_elements)

    # ============================== Setup functions: start ==============================

    def setup_conversion_objects(self):
        self._code_info = self.create_code_info()
        self._code_elements = code_elements.CodeElements()
        self._conversion_data = conversion_data.ConversionData(self._code_info)
        self.initialise_conversion_data()

    def create_code_info(self):
        cpp_filename = self._ccode.cfilename
        cpp_class_name = self._ccode.class_name
        return code_info.CodeInfo(cpp_filename, cpp_class_name)

    # Override to set required initial state
    def initialise_conversion_data(self):
        self._conversion_data.conversion_validation = DefaultConversionValidation()

    # ============================== Setup functions: end   ==============================

    # A chance to add specific data - override as required
    def set_function_specific_conversion_data(self, function_name):
        pass

    def add_includes(self):
        pass

    def convert_global_function(self):
        self._code_elements.global_function = conversion_funcs.convert_ccode_object(self._ccode.global_function, self._conversion_data)
        debug.line("convert_global_function", f"Converted C++ code [as_lines]...")
        debug.line("convert_global_function", self._code_elements.global_function.as_lines())

    def convert_data_members(self):
        for cmember in self._ccode.data_members:
            cppmember = conversion_funcs.convert_ccode_object(cmember, self._conversion_data)
            cppmember = data_member.DataMember(cppmember.decl_spec, cppmember.name + "_")

            # FOR NOW - MAKE ALL MEMBERS NON-CONST
            if cppmember.decl_spec.const_qualifier:
                cppmember.decl_spec.const_qualifier = ""
                debug.line("convert_data_members", f"*** SETTING DATA MEMBER TO NON-CONST *** cppmember=[{debug.as_debug_string(cppmember)}]")

            self._code_elements.add_data_member(cppmember)
            self._conversion_data.add_data_member_mapping(cmember, cppmember)
            debug.line("convert_data_members", f"cmember=[{debug.as_debug_string(cmember)}] cppmember=[{debug.as_debug_string(cppmember)}]")
            

    # Override to return True if the member function should be marked const
    def is_const_member_function(self, function_name):
        return False

    # Helper to ensure the function is converted correctly, including resetting the local conversion data!
    def to_cpp_function(self, func):
        self._conversion_data.reset_local_state()
        cpp_func = conversion_funcs.convert_ccode_object(func, self._conversion_data)
        if isinstance(cpp_func, member_function.MemberFunction):
            cpp_func.class_name = self._conversion_data.info.class_name
            cpp_func.set_is_const(self.is_const_member_function(func.funcsig.name))
        return cpp_func

    def convert_functions(self):
        for func in self._ccode.functions:
            self._conversion_data.reset_local_state()
            cppfunc = self.to_cpp_function(func)
            self._code_elements.add_function(cppfunc)
            self.dump_function("convert_functions", cppfunc)

    def convert_constructor_function(self):
        if self._ccode.constructor :
            constructor = self.to_cpp_function(self._ccode.constructor)
            self._code_elements.add_constructor(constructor)
            self.dump_function("convert_constructor_function", constructor)

    def convert_destructor_function(self):
        if self._ccode.destructor :
            destructor = self.to_cpp_function(self._ccode.destructor)
            self._code_elements.add_destructor(destructor)
            self.dump_function("convert_destructor_function", destructor)

    def convert_member_functions(self):
        debug.line("convert_member_functions", f"Converting member functions...")
        for func in self._ccode.member_functions:
            member_func = self.to_cpp_function(func)
            self._code_elements.add_member_function(member_func)
            self.dump_function("convert_member_functions", member_func)

    def convert_virtual_member_functions(self):
        debug.line("convert_virtual_member_functions", f"Converting virtual member functions...")
        for func in self._ccode.virtual_member_functions:
            virtual_member_func = self.to_cpp_function(func)
            self._code_elements.add_virtual_member_function(virtual_member_func)
            self.dump_function("convert_virtual_member_functions", virtual_member_func)

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body.as_lines())
        debug.line("dump_function", "================================================================================")
