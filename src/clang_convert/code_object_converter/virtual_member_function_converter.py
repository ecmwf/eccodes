
import utils.debug as debug
import code_object.virtual_member_function as virtual_member_function
import code_object_converter.member_function_converter as member_function_converter
import code_object_converter.conversion_funcs as conversion_funcs

class VirtualMemberFunctionConverter(member_function_converter.MemberFunctionConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, virtual_member_function.VirtualMemberFunction), f"Expected MemberFunction, got type=[{type(ccode_object)}]"

    def create_cpp_function(self, cpp_funcsig, cpp_body, conversion_pack):
        return virtual_member_function.VirtualMemberFunction(cpp_funcsig, cpp_body)

    def validate_cpp_function(self, cpp_function, conversion_pack):
        return conversion_pack.conversion_validation.validate_virtual_member_function(self._ccode_object, cpp_function)
