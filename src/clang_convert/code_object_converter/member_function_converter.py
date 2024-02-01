
import utils.debug as debug
import code_object.member_function as member_function
import code_object_converter.function_converter as function_converter
import code_object_converter.conversion_funcs as conversion_funcs

class MemberFunctionConverter(function_converter.FunctionConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, member_function.MemberFunction), f"Expected MemberFunction, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_funcsig = conversion_funcs.convert_ccode_object(self._ccode_object.funcsig, conversion_pack)
        cpp_body = conversion_funcs.convert_ccode_object(self._ccode_object.body, conversion_pack)

        cppmember_function = member_function.MemberFunction(cpp_funcsig, cpp_body)
        return conversion_pack.conversion_validation.validate_member_function(self._ccode_object, cppmember_function)
  