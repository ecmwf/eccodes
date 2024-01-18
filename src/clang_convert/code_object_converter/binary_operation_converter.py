
import utils.debug as debug
import code_object.binary_operation as binary_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class BinaryOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, binary_operation.BinaryOperation), f"Expected BinaryOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_left_operand = conversion_funcs.convert_ccode_object(self._ccode_object.left_operand, conversion_data)
        cpp_binary_op = self._ccode_object.binary_op
        cpp_right_operand = conversion_funcs.convert_ccode_object(self._ccode_object.right_operand, conversion_data)

        return binary_operation.BinaryOperation(cpp_left_operand, cpp_binary_op, cpp_right_operand)
