
import debug
import default.default_ccode_converter as default_ccode_converter
import code_object.cppfunction as cppfunction

# Converter for grib_accessor_ccode classes...
class GribAccessorCCodeConverter(default_ccode_converter.DefaultCCodeConverter):
    def __init__(self, ccode_instance) -> None:
        super().__init__(ccode_instance)

    # TODO when the class is available...
    @property
    def cpp_code_class(self):
        return super().cpp_code_class

    def convert_functions(self):
        self.convert_constructor_method()
        self.convert_destructor_method()
        self.convert_inherited_methods()
        self.convert_private_methods()
        self.convert_static_functions()

    def convert_constructor_method(self):
        constructor_funcsig = self.convert_cfunction_funcsig(self._ccode.constructor)
        constructor_body = self.convert_cfunction_body(self._ccode.constructor)
        constructor = cppfunction.CppFunction(constructor_funcsig, constructor_body)
        self.dump_function("convert_constructor_method", constructor)

    def convert_destructor_method(self):
        if not self._ccode.destructor:
            debug.line("convert_destructor_method", f"*** No destructor to convert! ***")
            return

        destructor_funcsig = self.convert_cfunction_funcsig(self._ccode.destructor)
        destructor_body = self.convert_cfunction_body(self._ccode.destructor)
        destructor = cppfunction.CppFunction(destructor_funcsig, destructor_body)
        self.dump_function("convert_destructor_method", destructor)

    def convert_inherited_methods(self):
        debug.line("convert_inherited_methods", f"Converting inherited methods...")
        for entry in self._ccode.virtual_member_functions:
            debug.line("convert_inherited_methods", f"Converting name=[{entry.funcsig.name}]")
            entry_funcsig = self.convert_cfunction_funcsig(entry)
            entry_body = self.convert_cfunction_body(entry)
            entry_cppfunction = cppfunction.CppFunction(entry_funcsig, entry_body)
            self.dump_function("convert_inherited_methods", entry_cppfunction)

    def convert_private_methods(self):
        debug.line("convert_private_methods", f"Converting private methods...")
        for entry in self._ccode.member_functions:
            debug.line("convert_private_methods", f"Converting name=[{entry.funcsig.name}]")
            entry_funcsig = self.convert_cfunction_funcsig(entry)
            entry_body = self.convert_cfunction_body(entry)
            entry_cppfunction = cppfunction.CppFunction(entry_funcsig, entry_body)
            self.dump_function("convert_private_methods", entry_cppfunction)

    def convert_static_functions(self):
        debug.line("convert_static_functions", f"Converting static functions...")
        for entry in self._ccode.functions:
            debug.line("convert_static_functions", f"Converting name=[{entry.funcsig.name}]")
            entry_funcsig = self.convert_cfunction_funcsig(entry)
            entry_body = self.convert_cfunction_body(entry)
            entry_cppfunction = cppfunction.CppFunction(entry_funcsig, entry_body)
            self.dump_function("convert_static_functions", entry_cppfunction)
