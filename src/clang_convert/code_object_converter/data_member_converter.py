
import utils.debug as debug
import code_object_converter.arg_converter as arg_converter
import code_object.data_member as data_member
import code_object.declaration_specifier as declaration_specifier
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import utils.standard_transforms as standard_transforms
from code_object_converter.conversion_pack.conversion_data_helper import *
from code_object.code_interface import NONE_VALUE

class DataMemberConverter(arg_converter.ArgConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, data_member.DataMember), f"Expected DataMember, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        self._conversion_pack = conversion_pack
        cmember = self._ccode_object
        debug.line("create_cpp_code_object", f"DataMemberConverter [IN] cmember=[{debug.as_debug_string(cmember)}]")

        cppmember = self._conversion_pack.conversion_data.cppdata_member_for_cdata_member(self._ccode_object)
        assert cppmember != NONE_VALUE

        if cppmember:
            return cppmember
        else:
            # Convert using the same rules as ArgConverter...
            cpp_decl_spec, _ = super().convert_funcbody_decl_spec(cmember.decl_spec)
            cpp_name = standard_transforms.transform_variable_name(cmember.name) + "_"

            cppmember = data_member.DataMember(cpp_decl_spec, cpp_name, cmember.mutable)

        # FOR NOW - MAKE ALL MEMBERS NON-CONST
        if cppmember.decl_spec.const_qualifier:
            cppmember.decl_spec.const_qualifier = ""
            debug.line("create_cpp_code_object", f"DataMemberConverter *** SETTING DATA MEMBER TO NON-CONST ***")

        self._conversion_pack.conversion_data.add_data_member_mapping(cmember, cppmember)
        debug.line("create_cpp_code_object", f"Adding data member mapping cmember=[{debug.as_debug_string(cmember)}] cppmember=[{debug.as_debug_string(cppmember)}]")

        debug.line("create_cpp_code_object", f"DataMemberConverter [OUT] cppmember=[{debug.as_debug_string(cppmember)}]")
        return conversion_pack.conversion_validation.validate_data_member(cmember, cppmember)
