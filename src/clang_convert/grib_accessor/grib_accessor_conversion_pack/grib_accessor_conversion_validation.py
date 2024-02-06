
import utils.debug as debug
import default.default_conversion_pack.default_conversion_validation as default_conversion_validation
import code_object.literal as literal
import code_object.return_statement as return_statement
import code_object.code_objects as code_objects
import code_object.compound_statement as compound_statement
import code_object.variable_declaration as variable_declaration
import code_object.binary_operation as binary_operation
import code_object.value_declaration_reference as value_declaration_reference
import code_object.if_statement as if_statement
import code_object.virtual_member_function as virtual_member_function
import code_object.constructor_function as constructor_function
import code_object.function_call as function_call
import code_object_converter.conversion_pack.arg_utils as arg_utils

from grib_accessor.grib_accessor_conversion_pack.grib_accessor_special_function_call_conversion import apply_special_function_call_conversions
from code_object.code_interface import NONE_VALUE
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_type_info as grib_accessor_type_info
from code_object_converter.conversion_funcs import as_commented_out_code
# Pass this to the conversion_data object to be accessed by the conversion routines
class GribAccessorConversionValidation(default_conversion_validation.DefaultConversionValidation):

    @property
    def type_info(self):
         return grib_accessor_type_info.GribAccessorTypeInfo()

    def validate_constructor_function(self, cconstructor_function, cppconstructor_function):
        # We'll add a line to set the class name in the name_ data member
        cpp_body = compound_statement.CompoundStatement()
        cpp_body.add_code_object(literal.Literal(f"name_ = \"{cppconstructor_function.class_name}\";"))

        for entry in cppconstructor_function.body.code_objects:
            cpp_body.add_code_object(entry)

        return constructor_function.ConstructorFunction(cppconstructor_function.funcsig,
                                                        cpp_body,
                                                        cppconstructor_function.class_name,
                                                        cppconstructor_function.super_class_name)

    def validate_virtual_member_function(self, cvirtual_member_function, cppvirtual_member_function):
        if cvirtual_member_function.funcsig.name in ["dump", "compare"]:
            cpp_body = compound_statement.CompoundStatement()
            
            cpp_body.add_code_object(as_commented_out_code("C++ implementation not yet available."))
            cpp_body.add_code_object(as_commented_out_code("Commented C body provided below for reference:\n"))
            
            for entry in cvirtual_member_function.body.code_objects:
                cpp_body.add_code_object(as_commented_out_code(entry))

            cpp_body.add_code_object(literal.Literal(f"\nreturn {self._conversion_data.info.super_class_name}::{cppvirtual_member_function.funcsig_as_call};"))
            return virtual_member_function.VirtualMemberFunction(cppvirtual_member_function.funcsig, cpp_body, cppvirtual_member_function.class_name)

        return super().validate_virtual_member_function(cvirtual_member_function, cppvirtual_member_function)


    def validate_function_call(self, cfunction_call, cppfunction_call):
        debug.line("validate_function_call", f"cfunction_call=[{debug.as_debug_string(cfunction_call)}] cppfunction_call=[{debug.as_debug_string(cppfunction_call)}]")

        special_function_call = apply_special_function_call_conversions(cfunction_call, cppfunction_call)
        if special_function_call:
            return special_function_call

        if cfunction_call.name == "sscanf":
            assert False, f"Need to add handling for sscanf from c_subs.py"

        return super().validate_function_call(cfunction_call, cppfunction_call)

    def validate_function_call_arg(self, calling_arg_value, target_arg):

        if "AccessorName" in target_arg.decl_spec.type:
            return literal.Literal(f"AccessorName({calling_arg_value.as_string()})")

        return super().validate_function_call_arg(calling_arg_value, target_arg)

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        if "GribStatus" in cppvariable_declaration.variable.as_string() and \
            not isinstance(cppvariable_declaration.value, function_call.FunctionCall):
            updated_cpp_variable_declaration = variable_declaration.VariableDeclaration(
                cppvariable_declaration.variable,
                literal.Literal(f"GribStatus{{{cppvariable_declaration.value.as_string()}}}"))
            return updated_cpp_variable_declaration

        return super().validate_variable_declaration(cvariable_declaration, cppvariable_declaration)

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):

        debug.line("validate_binary_operation", f"cbinary_operation=[{debug.as_debug_string(cbinary_operation)}] cppbinary_operation=[{debug.as_debug_string(cppbinary_operation)}]")

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

    def validate_return_statement(self, creturn_statement, cppreturn_statement):

        debug.line("validate_return_statement", f"creturn_statement=[{debug.as_debug_string(creturn_statement)}] cppreturn_statement=[{debug.as_debug_string(cppreturn_statement)}]")

        mapping = self._conversion_data.funcsig_mapping_for_current_cfuncname()
        if mapping:
            cppfunc_return_type = mapping.cppfuncsig.return_type.type
            cpparg = arg_utils.to_cpparg(cppreturn_statement.expression, self._conversion_data)
            updated_cpp_expression = None

            if cpparg:
                if cpparg.decl_spec.type != cppfunc_return_type:
                    updated_cpp_expression = literal.Literal(f"static_cast<GribStatus>({cpparg.name})")
            elif cppfunc_return_type == "GribStatus":
                cpp_expression = cppreturn_statement.expression.as_string()
                if cpp_expression == "0":
                    updated_cpp_expression = literal.Literal("GribStatus::SUCCESS")
                else:
                    updated_cpp_expression = literal.Literal(f"static_cast<GribStatus>({cpp_expression})")

            if updated_cpp_expression:
                return return_statement.ReturnStatement(updated_cpp_expression)

        return super().validate_return_statement(creturn_statement, cppreturn_statement)

    def validate_struct_member_access(self, cstruct_member_access, cppstruct_member_access):
        if cppstruct_member_access.name == "buffer_" and cppstruct_member_access.member.name == "data":
            # Just need to change ->data to .data()
            cppstruct_member_access.member.access = "."
            cppstruct_member_access.member.name += "()"
            return cppstruct_member_access

        return super().validate_struct_member_access(cstruct_member_access, cppstruct_member_access)

