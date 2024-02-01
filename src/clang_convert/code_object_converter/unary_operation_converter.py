
import utils.debug as debug
import code_object.unary_operation as unary_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.struct_member_access as struct_member_access

class UnaryOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, unary_operation.UnaryOperation), f"Expected UnaryOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_unary_op = conversion_funcs.convert_ccode_object(self._ccode_object.unary_op, conversion_pack)
        cpp_operand = conversion_funcs.convert_ccode_object(self._ccode_object.operand, conversion_pack)
        cpp_op_position = self._ccode_object.op_position

        cpp_unary_operation = unary_operation.UnaryOperation(cpp_unary_op, cpp_operand, cpp_op_position)
        return conversion_pack.conversion_validation.validate_unary_operation(self._ccode_object, cpp_unary_operation)
