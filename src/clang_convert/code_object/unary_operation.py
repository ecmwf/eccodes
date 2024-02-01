import utils.debug as debug
import code_object.code_interface as code_interface
import code_object.operation as operation

# Represent a unary operation of the form OP OPERAND
# Models the UNARY_OPERATOR CursorKind in libclang
#
# OP      is the operation (see class operation.Operation)
# OPERAND is the value OP is applied to (stored as a code_interface subclass)
#
# unary_op can be a string or an operation.Operation class
class UnaryOperation(code_interface.CodeInterface):
    def __init__(self, unary_op, operand, op_position="prefix") -> None:
        if isinstance(unary_op, str):
            self._unary_op = operation.Operation(unary_op)
        elif isinstance(unary_op, operation.Operation):
            self._unary_op = unary_op
        else:
            assert False, f"Expected unary_op [{unary_op}] to be type operation.Operation"

        self._operand = operand
        assert isinstance(self._operand, code_interface.CodeInterface), f"Operand must be a CodeInterface class"

        op_position_values = ["prefix", "postfix"]
        self._op_position = op_position
        assert isinstance(self._op_position, str), f"op_position must be a str, not [{type(self._op_position).__name__}]"
        assert self._op_position in ["prefix", "postfix"], f"op_position must be on of {op_position_values}, not=[{self._op_position}]"

    @property
    def unary_op(self):
        return self._unary_op

    @property
    def operand(self):
        return self._operand
    
    @property
    def op_position(self):
        return self._op_position

    def as_lines(self):
        lines = []
        lines.extend(self._operand.as_lines())
        if self._op_position == "prefix":
            lines[0] = self._unary_op.as_string() + lines[0]
        else:
            lines[-1] = lines[-1] + self._unary_op.as_string()

        if not lines[-1].endswith(";"):
            lines[-1] += ";"

        return lines