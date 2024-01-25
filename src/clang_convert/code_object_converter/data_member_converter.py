
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
        cpparg = super().create_cpp_code_object(conversion_pack)

        return data_member.DataMember(cpparg.decl_spec, cpparg.name, self._ccode_object.mutable)
