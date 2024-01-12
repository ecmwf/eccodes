
import debug
import transforms
import default.default_cast_parser as default_cast_parser
import code_object.cppfunction as cppfunction
import code_object_converter.cfuncsig_converter as cfuncsig_converter
import default.default_cppcode as default_cppcode

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class DefaultCCodeConverter:
    def __init__(self, ccode_instance) -> None:
        self._ccode = ccode_instance

    # Override to set the correct parser class as required by derived classes
    @property
    def cast_parser_class(self):
        return default_cast_parser.DefaultCASTParser

    # Override to set the correct CppCode class as required by derived classes
    @property
    def cpp_code_class(self):
        return default_cppcode.DefaultCppCode

    def convert(self):
        self.create_cpp_code()
        self.create_transforms()
        self.convert_global_declarations()
        self.convert_functions()

        return self._cppcode
    
    def create_cpp_code(self):
        self._cppcode = self.cpp_code_class()

    def create_transforms(self):
        self._transforms = transforms.Transforms()

    def convert_global_declarations(self):
        global_decl_ast_parser = self.cast_parser_class()
        global_decl_ccode_objects = global_decl_ast_parser.to_ccode_objects(self._ccode.global_declarations, self._transforms, self._ccode.macro_details)
        debug.line("convert_global_declarations", global_decl_ccode_objects.as_lines())

    # Helper to create the funcsig mapping and update the transforms
    def convert_cfunction_funcsig(self, cfunc):
        cppfuncsig = self._transforms.cppfuncsig_for_cfuncsig(cfunc.funcsig)
        if not cppfuncsig:
            cfuncsig_conv = cfuncsig_converter.CFuncSigConverter(cfunc.funcsig)
            mapping = cfuncsig_conv.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_funcsig_mappings(mapping)
            cppfuncsig = mapping.cppfuncsig
            assert cppfuncsig, f"Error creating cppfuncsig from cfuncsig=[{cfunc.funcsig.as_string()}]"

        return cppfuncsig

    def convert_cfunction_body(self, cfunc):
        function_body_ast_parser = self.cast_parser_class()
        function_body_ccode_objects = function_body_ast_parser.to_ccode_objects(cfunc.body, self._transforms, self._ccode.macro_details)
        return function_body_ccode_objects.as_lines()

    def convert_functions(self):
        for func in self._ccode.functions:
            cppfuncsig = self.convert_cfunction_funcsig(func)
            cppbody = self.convert_cfunction_body(func)
            cppfunc = cppfunction.CppFunction(cppfuncsig, cppbody)
            self.dump_function("convert_functions", cppfunc)

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body)
        debug.line("dump_function", "================================================================================")
