
import utils.debug as debug
import default.default_conversion_assistant as default_conversion_assistant
import code_object.literal as literal
import code_object.variable_declaration as variable_declaration

# Provides extra conversion functions, that can be specialised (e.g. for grib_accessor)
#
# Pass this to the conversion_data object to be accessed by the conversion routines
class GribAccessorConversionAssistant(default_conversion_assistant.DefaultConversionAssistant):

    def validate_function_call_args(self, calling_arg_value, target_arg):

        if "AccessorName" in target_arg.decl_spec.type:
            return literal.Literal(f"AccessorName({calling_arg_value.as_string()})")

        return super().validate_function_call_args(calling_arg_value, target_arg)

    def validate_variable_declaration(self, cpp_variable_declaration):

        if "GribStatus" in cpp_variable_declaration.variable.as_string():
            cpp_variable_declaration = variable_declaration.VariableDeclaration(
                cpp_variable_declaration.variable,
                literal.Literal(f"GribStatus{{{cpp_variable_declaration.value.as_string()}}}"))

        return cpp_variable_declaration