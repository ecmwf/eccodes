import code_object.literal as literal
import utils.debug as debug

def as_commented_out_code(ccode_object, prefix=""):
    comment_string = "// "
    if prefix:
        comment_string += f"[{prefix}] "
    comment_string += debug.as_debug_string(ccode_object)

    return literal.Literal(comment_string)