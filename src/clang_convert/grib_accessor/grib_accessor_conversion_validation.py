
import utils.debug as debug
import default.default_conversion_validation as default_conversion_validation
import code_object.literal as literal
import code_object.variable_declaration as variable_declaration
import code_object.function_call as function_call
from grib_accessor.conversion_validation.grib_accessor_special_function_call_conversion import apply_special_function_call_conversions

# Pass this to the conversion_data object to be accessed by the conversion routines
class GribAccessorConversionValidation(default_conversion_validation.DefaultConversionValidation):

    def validate_function_call(self, cfunction_call, cppfunction_call, callee_funcsig_mapping):
        special_function_call = apply_special_function_call_conversions(cfunction_call, cppfunction_call)
        if special_function_call:
            return special_function_call
        
        return super().validate_function_call(cfunction_call, cppfunction_call, callee_funcsig_mapping)

    def validate_function_call_arg(self, calling_arg_value, target_arg):

        if "AccessorName" in target_arg.decl_spec.type:
            return literal.Literal(f"AccessorName({calling_arg_value.as_string()})")

        return super().validate_function_call_arg(calling_arg_value, target_arg)

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        if "GribStatus" in cppvariable_declaration.variable.as_string():
            updated_cpp_variable_declaration = variable_declaration.VariableDeclaration(
                cppvariable_declaration.variable,
                literal.Literal(f"GribStatus{{{cppvariable_declaration.value.as_string()}}}"))
            return updated_cpp_variable_declaration

        return super().validate_variable_declaration(cvariable_declaration, cppvariable_declaration)
    
    def is_pointer_to_class_instance(self, arg_name):
         if arg_name in ["a"]:
             return True
         return super().is_pointer_to_class_instance(arg_name)