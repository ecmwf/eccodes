
import utils.debug as debug
import code_object.try_block as try_block
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class TryBlockConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, try_block.TryBlock), f"Expected TryBlock, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_try_statement = conversion_funcs.convert_ccode_object(self._ccode_object.try_statement, conversion_pack)
        cpp_catch_statements = conversion_funcs.convert_ccode_object(self._ccode_object.catch_statements, conversion_pack)

        cpptry_block = try_block.TryBlock(cpp_try_statement, cpp_catch_statements)
        return conversion_pack.conversion_validation.validate_try_block(self._ccode_object, cpptry_block)
