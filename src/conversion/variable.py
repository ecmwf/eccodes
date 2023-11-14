# Helpers for matching variables

class Variable:
    def __init__(self, pointer, name, index):
        self._pointer = pointer
        self._name = name
        self._index = index
        self._type = None

    @property
    def pointer(self):
        return self._pointer
    
    @property
    def name(self):
        return self._name
    
    @property
    def index(self):
        return self._index

    # Type can be optionally set when useful, but is not a core member    
    @property
    def type(self):
        return self._type
    
    @type.setter
    def type(self, value):
        self._type = value

    def as_string(self):
        varstring = self._pointer if self._pointer else ""
        varstring += self._name if self._name else ""
        varstring += self._index if self._index else ""
        return varstring

# Represents the match token when evaluating variables, eg = == != , ; etc
class MatchToken:
    def __init__(self, value) -> None:
        self._value = value
    
    @property
    def value(self):
        return self._value
    
    @property
    def is_assignment(self):
        return self._value == "="
    
    @property
    def is_comparison(self):
        return self._value in ["==", "!=", ">", ">=", "<", "<="]

    @property
    def is_boolean_test(self):
        return self._value in ["&&", "||"]

    @property
    def is_separator(self):
        return self._value  == ","

    @property
    def is_terminator(self):
        return self._value in [")", "[", "]", ";"]
    
    @property
    def is_maths_operator(self):
        return self._value in ["+", "-", "*", "/", "%"] #, "+=", "-=", "*=", "/=", "%="]
    
    # Generate a string representing the value in a format that can be added to a string,
    # i.e. there is a space before "=", ">=" etc but not ",", ";" etc
    def as_string(self):
        if self.is_separator or self.is_terminator:
            return self._value
        else:
            return " " + self._value
