
import utils.debug as debug
import code_object.operation as operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class OperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, operation.Operation), f"Expected Operation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        # By default we just return a copy of the value
        return operation.Operation(self._ccode_object.value)
