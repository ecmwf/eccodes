
import utils.debug as debug
import code_object.unary_operation as unary_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class UnaryOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, unary_operation.UnaryOperation), f"Expected UnaryOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_unary_op = conversion_funcs.convert_ccode_object(self._ccode_object.unary_op, conversion_data)
        cpp_operand = conversion_funcs.convert_ccode_object(self._ccode_object.operand, conversion_data)

        return unary_operation.UnaryOperation(cpp_unary_op, cpp_operand)