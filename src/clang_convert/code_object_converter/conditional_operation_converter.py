
import utils.debug as debug
import code_object.conditional_operation as conditional_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class ConditionalOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, conditional_operation.ConditionalOperation), f"Expected ConditionalOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        cpp_bool_expression = conversion_funcs.convert_ccode_object(self._ccode_object.bool_expression, conversion_data)
        cpp_true_expression = conversion_funcs.convert_ccode_object(self._ccode_object.true_expression, conversion_data)
        cpp_false_expression = conversion_funcs.convert_ccode_object(self._ccode_object.false_expression, conversion_data)

        return conditional_operation.ConditionalOperation(cpp_bool_expression, cpp_true_expression, cpp_false_expression)
