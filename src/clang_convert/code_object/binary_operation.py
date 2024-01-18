import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.operation as operation

# Represent a binary operation of the form LEFT_OPERAND OP RIGHT_OPERAND
# Models the BINARY_OPERATOR CursorKind in libclang
#
# LEFT_OPERAND  is the value before OP (stored as a code_interface subclass)
# OP            is the operation (see class operation.Operation)
# RIGHT_OPERAND is the value after OP (stored as a code_interface subclass)
#
# binary_op can be a string or an operation.Operation class
class BinaryOperation(code_interface.CodeInterface):
    def __init__(self, left_operand, binary_op, right_operand) -> None:
        if isinstance(binary_op, str):
            self._binary_op = operation.Operation(binary_op)
        elif isinstance(binary_op, operation.Operation):
            self._binary_op = binary_op
        else:
            assert False, f"Expected binary_op [{binary_op}] to be type operation.Operation"

        self._left_operand = left_operand
        self._right_operand = right_operand
        assert isinstance(self._left_operand, code_interface.CodeInterface), f"Left operand must be a CodeInterface class"
        assert isinstance(self._right_operand, code_interface.CodeInterface), f"Right operand must be a CodeInterface class"

    @property
    def left_operand(self):
        return self._left_operand

    @property
    def binary_op(self):
        return self._binary_op

    @property
    def right_operand(self):
        return self._right_operand

    def as_lines(self):
        lines = []
        lines.extend(self._left_operand.as_lines())
        lines[-1] += " " + self._binary_op.as_string()

        right_lines = []
        right_lines.extend(self._right_operand.as_lines())
        lines[-1] += " " + right_lines[0]

        lines.extend(right_lines[1:])

        # Determine whether to add a ;
        if self._binary_op.is_assignment():
            lines[-1] += ";"

        return lines