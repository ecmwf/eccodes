
import utils.debug as debug
import code_object.arg as arg
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import utils.standard_transforms as standard_transforms
from code_object_converter.supporting.conversion_data_helper import *

class ArgConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, arg.Arg), f"Expected arg.Arg, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        carg = self._ccode_object

        # 1: Check for an existing mapping of the carg
        cpp_arg = conversion_data.cpparg_for_carg(carg)

        if not cpp_arg:
            # 2: Check for a DeclSpec match
            cpp_decl_spec, match_type = conversion_data.closest_cppdecl_spec_for_cdecl_spec(carg.decl_spec)

            if match_type == DeclSpecMatchType.NONE:
                cpp_decl_spec = conversion_funcs.convert_ccode_object(carg.decl_spec, conversion_data)

            if match_type != DeclSpecMatchType.FULL:
                conversion_data.add_type_mapping(carg.decl_spec, cpp_decl_spec)
                debug.line("create_cpp_code_object", f"Arg conversion type mapping: carg.decl_spec=[{debug.as_debug_string(carg.decl_spec)}] cpp_decl_spec=[{debug.as_debug_string(cpp_decl_spec)}]")

            if cpp_decl_spec == declaration_specifier.DeclSpec.NONE:
                cpp_arg = arg.Arg.NONE
            else:
                cpp_name = standard_transforms.transform_variable_name(carg.name)
                cpp_is_func_arg = carg.is_func_arg

                cpp_arg = arg.Arg(cpp_decl_spec, cpp_name, cpp_is_func_arg)

            conversion_data.add_arg_mapping(carg, cpp_arg)
            debug.line("create_cpp_code_object", f"Arg conversion arg mapping: [{debug.as_debug_string(carg)}] -> [{debug.as_debug_string(cpp_arg)}]")

        return cpp_arg

