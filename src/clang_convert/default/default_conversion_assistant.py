
import utils.debug as debug
import code_object.unary_operation as unary_operation

# Provides extra conversion functions, that can be specialised (e.g. for grib_accessor)
#
# Pass this to the conversion_data object to be accessed by the conversion routines
class DefaultConversionAssistant:
    def __init__(self) -> None:
        pass

    # Check that the calling_arg_value (as a string) is correct when calling
    # as target_arg (Arg object)
    # For example, calling_arg_value of "foo" may need to become MyNamedType("foo")
    # Returns an updated calling_arg_value
    def validate_function_call_args(self, calling_arg_value, target_arg):

        if isinstance(calling_arg_value, unary_operation.UnaryOperation) and \
            calling_arg_value.unary_op.as_string() == "&":
                # For now, assume we don't call functions with addressof
                debug.line("validate_function_call_args", f"calling_arg_value=[{debug.as_debug_string(calling_arg_value)}] and target_arg=[{debug.as_debug_string(target_arg)}] - stripping first [&]")
                return unary_operation.UnaryOperation("", calling_arg_value.operand)
                # Check that the target arg is not now a reference
                '''if target_arg.decl_spec.pointer == "&":
                    # Yep - so we need to strip the & from the calling arg!
                    debug.line("validate_function_call_args", f"calling_arg_value=[{debug.as_debug_string(calling_arg_value)}] and target_arg=[{debug.as_debug_string(target_arg)}] - stripping first [&]")
                    return unary_operation.UnaryOperation("", calling_arg_value.operand)'''

        # Just return the passed in value!
        return calling_arg_value

    # Extra checks for variables (such as casting if required)
    def validate_variable_declaration(self, cpp_variable_declaration):
        # Just return the passed in value!
        return cpp_variable_declaration