
import utils.debug as debug
import code_object.binary_operation as binary_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.literal as literal
import code_object.arg as arg
import code_object.struct_member_access as struct_member_access
from code_object.code_interface import NONE_VALUE
from code_object_converter.conversion_utils import as_commented_out_code

class BinaryOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, binary_operation.BinaryOperation), f"Expected BinaryOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        self._conversion_data = conversion_pack.conversion_data
        cpp_left_operand = conversion_funcs.convert_ccode_object(self._ccode_object.left_operand, conversion_pack)
        cpp_binary_op = self._ccode_object.binary_op
        cpp_right_operand = conversion_funcs.convert_ccode_object(self._ccode_object.right_operand, conversion_pack)

        if cpp_left_operand is None or cpp_left_operand == NONE_VALUE:
            debug.line("create_cpp_code_object", f"cpp_left_operand=[{debug.as_debug_string(cpp_left_operand)}] => returning commented out code")
            return as_commented_out_code(self._ccode_object, f"Removed invalid binary operation")

        if cpp_right_operand is None or cpp_right_operand == NONE_VALUE:
            debug.line("create_cpp_code_object", f"cpp_right_operand=[{debug.as_debug_string(cpp_left_operand)}] => returning commented out code")
            return as_commented_out_code(self._ccode_object, f"Removed invalid binary operation")

        cppbinary_operation = binary_operation.BinaryOperation(cpp_left_operand, cpp_binary_op, cpp_right_operand)

        return conversion_pack.conversion_validation.validate_binary_operation(self._ccode_object, cppbinary_operation)

