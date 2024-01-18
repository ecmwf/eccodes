
import utils.debug as debug
import default.default_cast_parser as default_cast_parser
import cpp_code_object.cppfunction as cppfunction
import cpp_code_object.member_function as member_function
import cpp_code_object.virtual_member_function as virtual_member_function
import cpp_code_object.constructor_function as constructor_function
import cpp_code_object.destructor_function as destructor_function
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
        self.initialise_conversion_data()
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

    # Override to set required initial state
    def initialise_conversion_data(self):
        pass

    # A chance to add specific data - override as required
    def set_function_specific_conversion_data(self, function_name):
        pass

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
        function_body_cppcode_objects = conversion_funcs.convert_ccode_object_collection(function_body_ccode_objects, self._conversion_data)
        return function_body_cppcode_objects.as_lines()

    # Standard function conversion: returns: funcsig, body
    def convert_function_helper(self, cfunc):
        self.set_function_specific_conversion_data(cfunc.funcsig.name)
        cppfuncsig = self.convert_cfunction_funcsig(cfunc)
        cppbody = self.convert_cfunction_body(cfunc)
        return cppfuncsig, cppbody

    def convert_functions(self):
        for func in self._ccode.functions:
            cppfuncsig, cppbody = self.convert_function_helper(func)
            cppfunc = cppfunction.CppFunction(cppfuncsig, cppbody)
            self._cppcode.add_function(cppfunc)
            self.dump_function("convert_functions", cppfunc)

    def convert_constructor_function(self):
        if self._ccode.constructor :
            cppfuncsig, cppbody = self.convert_function_helper(self._ccode.constructor)
            constructor = constructor_function.ConstructorFunction(cppfuncsig, cppbody, self._cppcode.class_name)
            self._cppcode.add_constructor(constructor)
            self.dump_function("convert_constructor_function", constructor)

    def convert_destructor_function(self):
        if self._ccode.destructor :
            cppfuncsig, cppbody = self.convert_function_helper(self._ccode.destructor)
            destructor = destructor_function.DestructorFunction(cppfuncsig, cppbody, self._cppcode.class_name)
            self._cppcode.add_destructor(destructor)
            self.dump_function("convert_destructor_function", destructor)

    def convert_member_functions(self):
        debug.line("convert_member_functions", f"Converting member functions...")
        for entry in self._ccode.member_functions:
            cppfuncsig, cppbody = self.convert_function_helper(entry)
            member_func = member_function.MemberFunction(cppfuncsig, cppbody, self._cppcode.class_name)
            self._cppcode.add_member_function(member_func)
            self.dump_function("convert_member_functions", member_func)

    def convert_virtual_member_functions(self):
        debug.line("convert_virtual_member_functions", f"Converting virtual member functions...")
        for entry in self._ccode.virtual_member_functions:
            cppfuncsig, cppbody = self.convert_function_helper(entry)
            virtual_member_func = virtual_member_function.VirtualMemberFunction(cppfuncsig, cppbody, self._cppcode.class_name)
            self._cppcode.add_virtual_member_function(virtual_member_func)
            self.dump_function("convert_virtual_member_functions", virtual_member_func)

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body)
        debug.line("dump_function", "================================================================================")
