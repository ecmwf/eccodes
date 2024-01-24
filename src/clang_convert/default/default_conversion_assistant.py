
import utils.debug as debug

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
        # Just return the passed in value!
        return calling_arg_value
