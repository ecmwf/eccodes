
import utils.debug as debug
import code_object.destructor_function as destructor_function
import code_object_converter.member_function_converter as member_function_converter
import code_object_converter.conversion_funcs as conversion_funcs

class DestructorFunctionConverter(member_function_converter.MemberFunctionConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, destructor_function.DestructorFunction), f"Expected DestructorFunction, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_funcsig = conversion_funcs.convert_ccode_object(self._ccode_object.funcsig, conversion_pack)
        cpp_body = conversion_funcs.convert_ccode_object(self._ccode_object.body, conversion_pack)

        return destructor_function.DestructorFunction(cpp_funcsig, cpp_body)
  