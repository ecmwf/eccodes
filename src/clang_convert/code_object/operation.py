import debug
import code_object.code_interface as code_interface

# Represent an operation that can be performed on a variable
#
class Operation(code_interface.CodeInterface):
    def __init__(self, value=None) -> None:
        self._value = value

    # To add (consider):
    # - pre and post (e.g. increment)
    # - Unary vs binary (or covered by pre/post?)

    @property
    def value(self):
        return self._value

    def as_lines(self):
        return [f"{self._value}"]

    def is_assignment(self):
        return self._value in ["=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="]
    
    def is_inc_dec(self):
        return self._value in ["++", "--"]
    
    def is_arithmetic(self):
        return self._value in ["+", "-", "*", "/", "%", "~", "&", "|", "^", "<<", ">>"]

    def is_logical(self):
        return self._value in ["!", "&&", "||"]

    def is_comparison(self):
        return self._value in ["==", "!=", "<", ">", "<=", ">="]

    def is_member_access(self):
        return self._value in ["[", "*", "&", "->", ".", "->*", ".*"]

    def is_comma(self):
        return self._value in [","]

    def is_conditional(self):
        return self._value in ["?", ":"]
