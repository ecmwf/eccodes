
import utils.debug as debug
import default.default_conversion_assistant as default_conversion_assistant
import code_object.literal as literal

# Provides extra conversion functions, that can be specialised (e.g. for grib_accessor)
#
# Pass this to the conversion_data object to be accessed by the conversion routines
class GribAccessorConversionAssistant(default_conversion_assistant.DefaultConversionAssistant):

    def validate_function_call_args(self, calling_arg_value, target_arg):

        if "AccessorName" in target_arg.decl_spec.type:
            return literal.Literal(f"AccessorName({calling_arg_value.as_string()})")

        return super().validate_function_call_args(calling_arg_value, target_arg)
