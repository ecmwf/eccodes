
import re

# Useful string funcs

# Strip the final ";" from the string; returns the updated string
def strip_semicolon(code):
    if code.endswith(";"):
        return code[:-1]
    return code

# Returns True if the string contains a number
def is_number(string_value):
    try:
        float(string_value)
        return True
    except ValueError:
        return False