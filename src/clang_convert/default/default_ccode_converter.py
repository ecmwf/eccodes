
import utils.debug as debug
import cpp_code.cppcode as cppcode
import cpp_code.code_info as code_info
import cpp_code.code_elements as code_elements
import code_object_converter.conversion_pack.conversion_data as conversion_data
import code_object_converter.conversion_pack.container_utils as container_utils
import code_object_converter.conversion_pack.conversion_pack as conversion_pack
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.data_member as data_member
from default.default_conversion_pack.default_conversion_validation import DefaultConversionValidation
import default.default_conversion_pack.default_container_utils as default_container_utils

import code_object.member_function as member_function
import default.default_conversion_pack.default_type_info as default_type_info

from code_object.code_interface import NONE_VALUE

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class DefaultCCodeConverter:
    def __init__(self, ccode_instance) -> None:
        self._ccode = ccode_instance

    def convert(self):
        conv_pack = self.setup_conversion_pack()
        self.add_includes(conv_pack)
        self.convert_global_function(conv_pack)
        self.convert_data_members(conv_pack)

        # Now set the conversion data state to local for the rest of the conversion
        conv_pack.conversion_data.set_local_state()

        # Function conversion order is:
        # 1. Non-class functions
        # 2. Member functions, as they may be called by:
        # 3. Constructor (shouldn't call virtual member functions)
        # 4. Virtual funcs
        # 5. Destructor last!
        self.convert_functions(conv_pack)
        self.convert_member_functions(conv_pack)
        self.convert_constructor_function(conv_pack)
        self.convert_virtual_member_functions(conv_pack)
        self.convert_destructor_function(conv_pack)

        # Post-processing
        self.run_post_processing()

        self.validate_code_elements()

        return cppcode.CppCode(self._code_info, self._code_elements)

    # ============================== Setup functions: start ==============================

    def setup_conversion_pack(self):
        self._code_info = self.create_code_info()
        self._code_elements = code_elements.CodeElements()
        conv_data = self.create_conversion_data()
        conv_validation = self.create_conversion_validation()
        container_utils = self.create_container_utils()
        return conversion_pack.ConversionPack(conv_data, conv_validation, container_utils)

    def create_code_info(self):
        cpp_filename = self._ccode.cfilename
        cpp_class_name = self._ccode.class_name
        return code_info.CodeInfo(cpp_filename, cpp_class_name)
    
    def create_conversion_data(self):
        conv_data = conversion_data.ConversionData(self._code_info)
        self.init_conversion_data(conv_data)
        self.set_custom_conversion_data(conv_data)
        return conv_data

    def init_conversion_data(self, conv_data):
        for entry in self.type_info.class_instance_pointer_names():
            conv_data.add_self_class_pointer_name(entry)

        for entry in self.type_info.cpp_container_types():
            conv_data.add_container_type(entry)

    # Override to set required initial state
    def set_custom_conversion_data(self, conv_data):
        pass

    def create_conversion_validation(self):
        return DefaultConversionValidation()

    def create_container_utils(self):
        return default_container_utils.DefaultContainerUtils()

    # ============================== Setup functions: end   ==============================

    # Override to extend the type-info...
    @property
    def type_info(self):
         return default_type_info.DefaultTypeInfo()
    
    # A chance to add specific data - override as required
    def function_specific_conversion_pack_updates(self, cfunction_name, conv_pack):
        pass

    def add_includes(self, conv_pack):
        pass

    def convert_global_function(self, conv_pack):
        self._code_elements.global_function = conversion_funcs.convert_ccode_object(self._ccode.global_function, conv_pack)
        debug.line("convert_global_function", f"Converted C++ code [as_lines]...")
        debug.line("convert_global_function", self._code_elements.global_function.as_lines())

    def convert_data_members(self, conv_pack):
        for cmember in self._ccode.data_members:
            cppmember = conversion_funcs.convert_ccode_object(cmember, conv_pack)
            self._code_elements.add_data_member(cppmember)

    # Override to return True if the member function should be marked const
    def is_const_member_function(self, function_name):
        return False

    # Helper to ensure the function is converted correctly, including resetting the local conversion data!
    #
    # NOTE: ***** This function will return NONE_VALUE if the function cannot be converted to C++ *****
    #
    def to_cpp_function(self, func, conv_pack):
        conv_pack.conversion_data.reset_local_state(func.funcsig.name)
        self.function_specific_conversion_pack_updates(func.funcsig.name, conv_pack)
        cpp_func = conversion_funcs.convert_ccode_object(func, conv_pack)
        if isinstance(cpp_func, member_function.MemberFunction):
            cpp_func.class_name = conv_pack.conversion_data.info.class_name
            cpp_func.set_is_const(self.is_const_member_function(func.funcsig.name))

        return cpp_func

    def convert_functions(self, conv_pack):
        for func in self._ccode.functions:
            cppfunc = self.to_cpp_function(func, conv_pack)
            if cppfunc != NONE_VALUE:
                self._code_elements.add_function(cppfunc)
                self.dump_function("convert_functions", cppfunc)

    def convert_constructor_function(self, conv_pack):
        if self._ccode.constructor :
            constructor = self.to_cpp_function(self._ccode.constructor, conv_pack)
            if constructor != NONE_VALUE:
                self._code_elements.add_constructor(constructor)
                self.dump_function("convert_constructor_function", constructor)

    def convert_destructor_function(self, conv_pack):
        if self._ccode.destructor :
            destructor = self.to_cpp_function(self._ccode.destructor, conv_pack)
            if destructor != NONE_VALUE:
                self._code_elements.add_destructor(destructor)
                self.dump_function("convert_destructor_function", destructor)

    def convert_member_functions(self, conv_pack):
        debug.line("convert_member_functions", f"Converting member functions...")
        for func in self._ccode.member_functions:
            member_func = self.to_cpp_function(func, conv_pack)
            if member_func != NONE_VALUE:
                self._code_elements.add_member_function(member_func)
                self.dump_function("convert_member_functions", member_func)

    def convert_virtual_member_functions(self, conv_pack):
        debug.line("convert_virtual_member_functions", f"Converting virtual member functions...")
        for func in self._ccode.virtual_member_functions:
            virtual_member_func = self.to_cpp_function(func, conv_pack)
            if virtual_member_func != NONE_VALUE:
                self._code_elements.add_virtual_member_function(virtual_member_func)
                self.dump_function("convert_virtual_member_functions", virtual_member_func)

    # ============================== Post-processing: begin ==============================

    def run_post_processing(self):
        self.post_process_function_calls()

    # Override as required...
    def post_process_function_calls(self):
        pass

    # ============================== Post-processing: end   ==============================

    # Override for any final validation of the new cppcode object
    def validate_code_elements(self):
        pass

    # Helper for consistent debug output!
    def dump_function(self, def_name, cppfunc):
        debug.line("dump_function", "================================================================================")
        debug.line("dump_function", f"Source=[{def_name}]")
        debug.line("dump_function", "--------------------------------------------------------------------------------")
        debug.line("dump_function", cppfunc.funcsig.as_string())
        debug.line("dump_function", cppfunc.body.as_lines())
        debug.line("dump_function", "================================================================================")
