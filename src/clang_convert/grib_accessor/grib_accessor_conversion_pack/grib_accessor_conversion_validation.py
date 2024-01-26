
import utils.debug as debug
import default.default_conversion_pack.default_conversion_validation as default_conversion_validation
import code_object.literal as literal
import code_object.variable_declaration as variable_declaration
import code_object.value_declaration_reference as value_declaration_reference
import code_object.binary_operation as binary_operation
from grib_accessor.grib_accessor_conversion_pack.grib_accessor_special_function_call_conversion import apply_special_function_call_conversions
from code_object.code_interface import NONE_VALUE
import code_object.if_statement as if_statement

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

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):
        if cppbinary_operation.left_operand.as_string() == "flags_":
            if cppbinary_operation.right_operand.as_string().startswith("GribAccessorFlag"):
                updated_right_operand = literal.Literal(f"toInt({cppbinary_operation.right_operand.as_string()})")
                return binary_operation.BinaryOperation(cppbinary_operation.left_operand, cppbinary_operation.binary_op, updated_right_operand)

        return super().validate_binary_operation(cbinary_operation, cppbinary_operation)

    def validate_if_statement(self, cif_statement, cppif_statement):
        # Update if(x) statement when type of x is enum GribStatus
        if isinstance(cppif_statement.expression, value_declaration_reference.ValueDeclarationReference):
            expression_value = cppif_statement.expression.as_string()
            cpparg = self._conversion_data.funcbody_cpparg_for_carg_name(expression_value)
            if cpparg and cpparg != NONE_VALUE and cpparg.decl_spec.type == "GribStatus":
                updated_expression = literal.Literal(f"isError({cpparg.name})")
                updated_cppif_statement = if_statement.IfStatement(updated_expression, cppif_statement.action)
                debug.line("validate_if_statement", f"updated_cppif_statement=[{debug.as_debug_string(updated_cppif_statement)}]")
                return updated_cppif_statement

        return super().validate_if_statement(cif_statement, cppif_statement)


    def is_pointer_to_class_instance(self, arg_name):
         if arg_name in ["a"]:
             return True
         return super().is_pointer_to_class_instance(arg_name)

    def is_cpp_container_type(self, cppdecl_spec):
        for type in [
            "AccessorDataBuffer",
        ]:
             if type in cppdecl_spec.type:
                  return True

        return super().is_cpp_container_type(cppdecl_spec)
