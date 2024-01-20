
import utils.debug as debug
import code_object.struct_arg as struct_arg
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class StructArgConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, struct_arg.StructArg), f"Expected StructArg, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_name = conversion_funcs.convert_ccode_object(self._ccode_object.name, conversion_data)
        cpp_members=[]
        for member in self._ccode_object.members:
            cpp_member = conversion_funcs.convert_ccode_object(member, conversion_data)
            cpp_members.append(cpp_member)

        return struct_arg.StructArg(cpp_name, cpp_members)
