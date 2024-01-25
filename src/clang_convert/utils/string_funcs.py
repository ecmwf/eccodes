
import re

# Useful string funcs

# Strip the final ";" from the string; returns the updated string
def strip_semicolon(code):
    if code.endswith(";"):
        return code[:-1]
    return code
