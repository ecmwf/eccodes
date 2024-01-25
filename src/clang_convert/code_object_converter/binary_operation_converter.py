
import utils.debug as debug
import code_object.binary_operation as binary_operation
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.literal as literal
import code_object.arg as arg
import code_object.struct_member_access as struct_member_access

class BinaryOperationConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, binary_operation.BinaryOperation), f"Expected BinaryOperation, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data
        cpp_left_operand = conversion_funcs.convert_ccode_object(self._ccode_object.left_operand, conversion_data)
        cpp_binary_op = self._ccode_object.binary_op
        cpp_right_operand = conversion_funcs.convert_ccode_object(self._ccode_object.right_operand, conversion_data)

        if isinstance(cpp_right_operand, literal.Literal):
            cpptype = self.get_type(cpp_right_operand)
            if cpptype:
                assert False

        return binary_operation.BinaryOperation(cpp_left_operand, cpp_binary_op, cpp_right_operand)
    
    def get_type(self, right_operand):
        if isinstance(right_operand, arg.Arg):
            return right_operand.decl_spec.type
        if isinstance(right_operand, struct_member_access.StructMemberAccess):
            cppmember = self._conversion_data.cppdata_member_for_cdata_member_name(right_operand.name)
            return cppmember.decl_spec.type
        return None
