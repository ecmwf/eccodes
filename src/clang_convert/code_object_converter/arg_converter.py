
import utils.debug as debug
import code_object.arg as arg
import code_object.declaration_specifier as declaration_specifier
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import utils.standard_transforms as standard_transforms
from code_object_converter.supporting.conversion_data_helper import *

class ArgConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, arg.Arg), f"Expected arg.Arg, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data

        debug.line("create_cpp_code_object", f"ArgConverter [IN] self._ccode_object=[{debug.as_debug_string(self._ccode_object)}] is_func_arg=[{self._ccode_object.is_func_arg}]")

        if self._ccode_object.is_func_arg:
            cpp_arg = self.convert_funcsig_arg()
        else:
            cpp_arg = self.convert_funcbody_arg()

        debug.line("create_cpp_code_object", f"ArgConverter [OUT] cpp_arg=[{debug.as_debug_string(cpp_arg)}]")
        return cpp_arg

    def convert_funcsig_arg(self):
        carg = self._ccode_object

        debug.line("convert_funcsig_arg", f"--[1]-- carg=[{debug.as_debug_string(carg)}]")

        # 1. Check for existing arg mappings
        cpp_arg = self._conversion_data.funcsig_cpparg_for_carg(carg)
        if cpp_arg:
            return cpp_arg

        debug.line("convert_funcsig_arg", f"--[2]-- carg=[{debug.as_debug_string(cpp_arg)}]")

        # 2. Check if the C type should not be converted (e.g. grib_handle*)
        test_decl_spec = declaration_specifier.DeclSpec.from_instance(carg.decl_spec)
        cpp_decl_spec, match_type = self.convert_funcsig_decl_spec(test_decl_spec)

        debug.line("convert_funcsig_arg", f"--[3]-- test_decl_spec=[{debug.as_debug_string(test_decl_spec)}] cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}] match_type=[{match_type}]")

        if cpp_decl_spec == declaration_specifier.DeclSpec.NONE:
            cpp_arg = arg.Arg.NONE
        else:
            if match_type != DeclSpecMatchType.FULL:
                # 3. Convert the type with the pointer removed, as the default behaviour for a funcsig
                #    arg is to swap * for & (e.g. double* becomes double&)
                test_decl_spec.pointer = ""
                cpp_decl_spec, _ = self.convert_funcsig_decl_spec(test_decl_spec)
                if carg.decl_spec.pointer:
                    cpp_decl_spec.pointer = "&"
            cpp_arg = self.create_funcsig_cpparg(cpp_decl_spec)

        debug.line("convert_funcsig_arg", f"--[4]-- cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}] cpp_arg=[{debug.as_debug_string(cpp_arg)}]")

        if not match_type == DeclSpecMatchType.FULL:
            self._conversion_data.add_funcsig_type_mapping(carg.decl_spec, cpp_decl_spec)
            debug.line("convert_funcsig_arg", f"Arg conversion type mapping: carg.decl_spec=[{debug.as_debug_string(carg.decl_spec)}] cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}]")

        return cpp_arg

    def convert_funcsig_decl_spec(self, test_decl_spec):
        assert test_decl_spec

        cpp_decl_spec, match_type = self._conversion_data.closest_funcsig_cppdecl_spec_for_cdecl_spec(test_decl_spec)

        if match_type == DeclSpecMatchType.NONE:
            cpp_decl_spec = conversion_funcs.convert_ccode_object(test_decl_spec, self._conversion_data)

        return cpp_decl_spec, match_type

    def create_funcsig_cpparg(self, new_decl_spec):
        carg = self._ccode_object

        cpp_name = standard_transforms.transform_variable_name(carg.name)
        cpp_is_func_arg = carg.is_func_arg

        cpp_arg = arg.Arg(new_decl_spec, cpp_name, cpp_is_func_arg)

        self._conversion_data.add_funcsig_arg_mapping(carg, cpp_arg)
        debug.line("create_funcsig_cpparg", f"Arg conversion arg mapping: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpp_arg)}]")

        return cpp_arg
    def convert_funcbody_arg(self):
        carg = self._ccode_object

        debug.line("convert_funcbody_arg", f"==[1]== carg=[{debug.as_debug_string(carg)}]")

        # 1. Check for existing arg mappings
        cpp_arg = self._conversion_data.funcbody_cpparg_for_carg(carg)
        if cpp_arg:
            return cpp_arg
        
        debug.line("convert_funcbody_arg", f"==[2]== carg=[{debug.as_debug_string(cpp_arg)}]")
        
        # 2. Convert the type 
        cpp_decl_spec, match_type = self.convert_funcbody_decl_spec(carg.decl_spec)

        debug.line("convert_funcbody_arg", f"==[3]== cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}] match_type=[{match_type}]")

        if cpp_decl_spec == declaration_specifier.DeclSpec.NONE:
            cpp_arg = arg.Arg.NONE
        else:
            # 3. If we didn't match the pointer, and the converted pointer is not "", then convert the type to a vector
            if (not match_type.value and DeclSpecMatchType.POINTER) and cpp_decl_spec.pointer:
                cpp_decl_spec.type = f"std::vector<{cpp_decl_spec.type}>"
                cpp_decl_spec.pointer = ""
                debug.line("convert_funcbody_arg", f"Type changed to std::vector, cpp_decl_spec=[{cpp_decl_spec}]")
            cpp_arg = self.create_funcbody_cpparg(cpp_decl_spec)

        debug.line("convert_funcbody_arg", f"==[4]== cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}] cpp_arg=[{debug.as_debug_string(cpp_arg)}]")

        if not match_type == DeclSpecMatchType.FULL:
            self._conversion_data.add_funcbody_type_mapping(carg.decl_spec, cpp_decl_spec)
            debug.line("convert_funcbody_arg", f"Arg conversion type mapping: carg.decl_spec=[{debug.as_debug_string(carg.decl_spec)}] cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}]")

        return cpp_arg

    def convert_funcbody_decl_spec(self, test_decl_spec):
        assert test_decl_spec

        cpp_decl_spec, match_type = self._conversion_data.closest_funcbody_cppdecl_spec_for_cdecl_spec(test_decl_spec)

        if match_type == DeclSpecMatchType.NONE:
            cpp_decl_spec = conversion_funcs.convert_ccode_object(test_decl_spec, self._conversion_data)

        return cpp_decl_spec, match_type

    def create_funcbody_cpparg(self, new_decl_spec):
        carg = self._ccode_object

        cpp_name = standard_transforms.transform_variable_name(carg.name)
        cpp_is_func_arg = carg.is_func_arg

        cpp_arg = arg.Arg(new_decl_spec, cpp_name, cpp_is_func_arg)

        self._conversion_data.add_funcbody_arg_mapping(carg, cpp_arg)
        debug.line("create_funcbody_cpparg", f"Arg conversion arg mapping: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpp_arg)}]")

        return cpp_arg
        
