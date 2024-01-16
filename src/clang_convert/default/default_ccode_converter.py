
import utils.debug as debug
import default.default_cast_parser as default_cast_parser
import code_object.cppfunction as cppfunction
import default.default_cppcode as default_cppcode
import code_object_converter.conversion_data as conversion_data 
import code_object_converter.conversion_funcs as conversion_funcs
import code_object_converter.funcsig_converter as funcsig_converter

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class DefaultCCodeConverter:
    def __init__(self, ccode_instance) -> None:
        self._ccode = ccode_instance

    # Override to set the correct parser class as required by derived classes
    @property
    def cast_parser_class(self):
        return default_cast_parser.DefaultCASTParser

    # Override to create the required CppCode class
    def create_cpp_code(self):
        cpp_filename = self._ccode.cfilename
        cpp_class_name = self._ccode.class_name
        self._cppcode = default_cppcode.DefaultCppCode(cpp_filename, cpp_class_name)

    def convert(self):
        self.create_cpp_code()
        self.create_conversion_data()
        self.convert_global_declarations()
        self.convert_data_members()
        self.convert_functions()
        self.convert_constructor_function()
        self.convert_destructor_function()
        self.convert_member_functions()
        self.convert_virtual_member_functions()

        return self._cppcode

    def create_conversion_data(self):
        self._conversion_data = conversion_data.ConversionData()

        # TODO - ADD funcsig mappings here...

    def convert_global_declarations(self):
        global_decl_ast_parser = self.cast_parser_class()
        global_decl_ccode_objects = global_decl_ast_parser.to_ccode_objects(self._ccode.global_declarations, self._conversion_data, self._ccode.macro_details)
        debug.line("convert_global_declarations", global_decl_ccode_objects.as_lines())
        global_decl_cpp_code_objects = conversion_funcs.convert_ccode_object_collection(global_decl_ccode_objects, self._conversion_data)
        self._cppcode.add_global_declaration(global_decl_cpp_code_objects)
        debug.line("convert_global_declarations", f"Converted C++ code [as_lines]...")
        debug.line("convert_global_declarations", global_decl_cpp_code_objects.as_lines())

    def convert_data_members(self):
        for cmember in self._ccode.data_members:
            pass # TO DO

    # Helper to create the funcsig mapping and update the transforms
    def convert_cfunction_funcsig(self, cfunc):
        cppfuncsig = conversion_funcs.convert_ccode_object(cfunc.funcsig, self._conversion_data)
        assert cppfuncsig, f"Error creating cppfuncsig from cfuncsig=[{cfunc.funcsig.as_string()}]"
        return cppfuncsig

    def convert_cfunction_body(self, cfunc):
        function_body_ast_parser = self.cast_parser_class()
        function_body_ccode_objects = function_body_ast_parser.to_ccode_objects(cfunc.body, self._conversion_data, self._ccode.macro_details)
        return function_body_ccode_objects.as_lines()

    # Use this if no other version provided...
    def default_convert_function(self, cfunc):
        cppfuncsig = self.convert_cfunction_funcsig(cfunc)
        cppbody = self.convert_cfunction_body(cfunc)
        cppfunc = cppfunction.CppFunction(cppfuncsig, cppbody)
        return cppfunc

    def convert_functions(self):
        for func in self._ccode.functions:
            cppfunc = self.default_convert_function(func)
            self._cppcode.add_function(cppfunc)
            self.dump_function("Default convert_functions", cppfunc)

    def convert_constructor_function(self):
        if self._ccode.constructor :
            constructor = self.default_convert_function(self._ccode.constructor)
            self._cppcode.add_constructor(constructor)
            self.dump_function("Default convert_constructor_function", constructor)

    def convert_destructor_function(self):
        if self._ccode.destructor :
            destructor = self.default_convert_function(self._ccode.destructor)
            self._cppcode.add_destructor(destructor)
            self.dump_function("Default convert_destructor_function", destructor)

    def convert_member_functions(self):
        debug.line("convert_member_functions", f"Converting member functions...")
        for entry in self._ccode.member_functions:
            member_func = self.default_convert_function(entry)
            self._cppcode.add_member_function(member_func)
            self.dump_function("Default convert_member_functions", member_func)

    def convert_virtual_member_functions(self):
        debug.line("convert_virtual_member_functions", f"Converting virtual member functions...")
        for entry in self._ccode.virtual_member_functions:
            virtual_member_func = self.default_convert_function(entry)
            self._cppcode.add_virtual_member_function(virtual_member_func)
            self.dump_function("Default convert_virtual_member_functions", virtual_member_func)

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body)
        debug.line("dump_function", "================================================================================")
