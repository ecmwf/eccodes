
import utils.debug as debug
import default.default_cppcode as default_cppcode
import code_object_converter.supporting.conversion_data as conversion_data 
import code_object_converter.conversion_funcs as conversion_funcs

import code_object.global_function as global_function

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class DefaultCCodeConverter:
    def __init__(self, ccode_instance) -> None:
        self._ccode = ccode_instance

    # Override to create the required CppCode class
    def create_cpp_code(self):
        cpp_filename = self._ccode.cfilename
        cpp_class_name = self._ccode.class_name
        self._cppcode = default_cppcode.DefaultCppCode(cpp_filename, cpp_class_name)

    # Entry point...
    def convert(self):
        self.create_cpp_code()
        self.create_conversion_data()
        self.initialise_conversion_data()
        self.convert_global_function()
        self.convert_data_members()
        self.convert_functions()
        self.convert_constructor_function()
        self.convert_destructor_function()
        self.convert_member_functions()
        self.convert_virtual_member_functions()

        return self._cppcode

    def create_conversion_data(self):
        self._conversion_data = conversion_data.ConversionData()

    # Override to set required initial state
    def initialise_conversion_data(self):
        pass

    # A chance to add specific data - override as required
    def set_function_specific_conversion_data(self, function_name):
        pass

    def convert_global_function(self):
        self._cppcode.global_function = conversion_funcs.convert_ccode_object(self._ccode.global_function, self._conversion_data)
        debug.line("convert_global_declarations", f"Converted C++ code [as_lines]...")
        debug.line("convert_global_declarations", self._cppcode.global_function.as_lines())

    def convert_data_members(self):
        for cmember in self._ccode.data_members:
            pass # TO DO

    def convert_functions(self):
        for func in self._ccode.functions:
            cppfunc = conversion_funcs.convert_ccode_object(func, self._conversion_data)
            self._cppcode.add_function(cppfunc)
            self.dump_function("convert_functions", cppfunc)

    def convert_constructor_function(self):
        if self._ccode.constructor :
            constructor = conversion_funcs.convert_ccode_object(self._ccode.constructor, self._conversion_data)
            self._cppcode.add_constructor(constructor)
            self.dump_function("convert_constructor_function", constructor)

    def convert_destructor_function(self):
        if self._ccode.destructor :
            destructor = conversion_funcs.convert_ccode_object(self._ccode.destructor, self._conversion_data)
            self._cppcode.add_destructor(destructor)
            self.dump_function("convert_destructor_function", destructor)

    def convert_member_functions(self):
        debug.line("convert_member_functions", f"Converting member functions...")
        for func in self._ccode.member_functions:
            member_func = conversion_funcs.convert_ccode_object(func, self._conversion_data)
            self._cppcode.add_member_function(member_func)
            self.dump_function("convert_member_functions", member_func)

    def convert_virtual_member_functions(self):
        debug.line("convert_virtual_member_functions", f"Converting virtual member functions...")
        for func in self._ccode.virtual_member_functions:
            virtual_member_func = conversion_funcs.convert_ccode_object(func, self._conversion_data)
            self._cppcode.add_virtual_member_function(virtual_member_func)
            self.dump_function("convert_virtual_member_functions", virtual_member_func)

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body.as_lines())
        debug.line("dump_function", "================================================================================")
