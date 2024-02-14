
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
import code_object.cast_expression as cast_expression
import code_object.macro_instantation as macro_instantation
import code_object.struct_member_access as struct_member_access
from utils.string_funcs import strip_semicolon

from grib_accessor.grib_accessor_conversion_pack.grib_accessor_special_function_call_conversion import special_function_name_mapping
from code_object.code_interface import NONE_VALUE
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_type_info as grib_accessor_type_info
from code_object_converter.conversion_utils import as_commented_out_code
import re

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
            cpp_body.add_code_object(as_commented_out_code("Current C++ conversion provided below (disabled, for reference only):\n"))
            cpp_body.add_code_object(literal.Literal(f"#if 0"))
            
            for entry in cppvirtual_member_function.body.code_objects:
                cpp_body.add_code_object(entry)

            cpp_body.add_code_object(literal.Literal(f"#endif // 0"))

            cpp_body.add_code_object(literal.Literal(f"\nreturn {self._conversion_data.info.super_class_name}::{cppvirtual_member_function.funcsig_as_call};"))
            return virtual_member_function.VirtualMemberFunction(cppvirtual_member_function.funcsig, cpp_body, cppvirtual_member_function.class_name)

        return super().validate_virtual_member_function(cvirtual_member_function, cppvirtual_member_function)


    def validate_function_call(self, cfunction_call, cppfunction_call):
        debug.line("validate_function_call", f"cfunction_call=[{debug.as_debug_string(cfunction_call)}] cppfunction_call=[{debug.as_debug_string(cppfunction_call)}]")

        special_function_call = self.apply_special_function_call_conversions(cfunction_call, cppfunction_call)
        if special_function_call:
            return special_function_call

        if cppfunction_call.name.startswith("super->"):
            updated_func_name = cppfunction_call.name.replace("super->", f"{self._conversion_data.info.super_class_name}::")
            updated_cppfunction_call = function_call.FunctionCall(updated_func_name, cppfunction_call.args)

            debug.line("validate_function_call", f"updated func name: [{debug.as_debug_string(cppfunction_call)}]->[{debug.as_debug_string(updated_cppfunction_call)}]")
            return updated_cppfunction_call
        
        if cfunction_call.name == "sscanf":
            # Need to convert to using conversion helper function:
            #   template <typename... Args>
            #   int scanString(std::string buffer, size_t offset, std::string format, Args&... args)
            # offset is used when indexing into buffer, e.g. val + 2*i, otherwise set to 0
            cppargs = []
            cppname = arg_utils.extract_function_call_name(cppfunction_call.args[0])

            if "[" in cppname:
                m = re.match(r"([^\[]+)\[([^\]]+)\]", cppname)
                if m:
                    cppargs.append(literal.Literal(m.group(1)))
                    cppargs.append(literal.Literal(m.group(2)))
                else:
                    cppargs.append(cppfunction_call.args[0])
                    cppargs.append(literal.Literal("0"))

            for cpparg in cppfunction_call.args[1:]:
                cppargs.append(cpparg)
            
            updated_cppfunction_call = function_call.FunctionCall("scanString", cppargs)

            debug.line("validate_function_call", f"sscanf conversion:[{debug.as_debug_string(cppfunction_call)}]->[{debug.as_debug_string(updated_cppfunction_call)}]")
            return updated_cppfunction_call

        return super().validate_function_call(cfunction_call, cppfunction_call)

    def validate_function_call_arg(self, calling_arg_value, target_arg):

        if "AccessorName" in target_arg.decl_spec.type:
            return literal.Literal(f"AccessorName({arg_utils.extract_function_call_name(calling_arg_value)})")

        return super().validate_function_call_arg(calling_arg_value, target_arg)

    def apply_special_function_call_conversions(self, cfunction_call, cppfunction_call):

        # Convert grib_arguments_get_XXX calls
        grib_arguments_get_cname = "grib_arguments_get_"
        if cfunction_call.name.startswith(grib_arguments_get_cname):
            cpptype = cfunction_call.name[len(grib_arguments_get_cname):]
            if cpptype == "name":
                cpptype = "std::string"
            elif cpptype == "expression":
                cpptype = "GribExpressionPtr"
        
            arg_string = strip_semicolon(cfunction_call.args[2].as_string())
            arg_entry = literal.Literal(f"initData.args[{arg_string}].second")
            return function_call.FunctionCall(f"std::get<{cpptype}>", [arg_entry])

        # If we're calling grib_XXX which is a virtual member function, and the first argument is "a", then we're actually calling ourself!
        if cfunction_call.name.startswith("grib_"):
            updated_cfuncname = cfunction_call.name[5:]
            if self._conversion_data.is_virtual_member_function(updated_cfuncname) and \
               len(cfunction_call.args) > 0 and cfunction_call.args[0].as_string() == "a":
                mapping = self._conversion_data.funcsig_mapping_for_cfuncname(updated_cfuncname)
                if mapping:
                    updated_cppfunction_call = function_call.FunctionCall(mapping.cppfuncsig.name, cppfunction_call.args[1:])
                    updated_cppfunction_call = self.validate_function_call_args(updated_cppfunction_call, mapping.cppfuncsig)
                    debug.line("apply_special_function_call_conversions", f"Updated C++ function call=[{debug.as_debug_string(cppfunction_call)}] to [{debug.as_debug_string(updated_cppfunction_call)}]")
                    return updated_cppfunction_call

        for cfuncname, cppfuncname in special_function_name_mapping.items():
            if cfunction_call.name == cfuncname:
                if cppfuncname:
                    return function_call.FunctionCall(cppfuncname, cppfunction_call.args)
                else:
                    return as_commented_out_code(cfunction_call, f"Removed call to {cfuncname}")

        return None

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        if "GribStatus" in cppvariable_declaration.variable.as_string() and \
           not isinstance(cppvariable_declaration.value, function_call.FunctionCall) and \
           not cppvariable_declaration.value.as_string().startswith("GribStatus"):
            updated_cpp_variable_declaration = variable_declaration.VariableDeclaration(
                cppvariable_declaration.variable,
                literal.Literal(f"GribStatus{{{cppvariable_declaration.value.as_string()}}}"))
            return updated_cpp_variable_declaration

        return super().validate_variable_declaration(cvariable_declaration, cppvariable_declaration)

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):

        debug.line("validate_binary_operation", f"cbinary_operation=[{debug.as_debug_string(cbinary_operation)}] cppbinary_operation=[{debug.as_debug_string(cppbinary_operation)}]")

        cppleft = cppbinary_operation.left_operand
        cppbinary_op = cppbinary_operation.binary_op
        cppright = cppbinary_operation.right_operand

        if cppbinary_operation.left_operand.as_string() == "flags_":
            if cppright.as_string().startswith("GribAccessorFlag"):
                updated_right_operand = literal.Literal(f"toInt({cppright.as_string()})")
                return binary_operation.BinaryOperation(cppleft, cppbinary_op, updated_right_operand)
        
        if isinstance(cppright, cast_expression.CastExpression):
            cpp_right_expression = cppright.expression

            if isinstance(cpp_right_expression, function_call.FunctionCall) and cpp_right_expression.name in ["gribContextMalloc", "gribContextRealloc"]:
                # For now, we'll assume we're allocating a container (may need to revisit)
                cpp_alloc = literal.Literal(f"{arg_utils.extract_name(cppleft)}.resize({cpp_right_expression.arg_string});")
                debug.line("validate_binary_operation", f"Changed allocation operation from=[{debug.as_debug_string(cppbinary_operation)}] to [{debug.as_debug_string(cpp_alloc)}]")
                return cpp_alloc

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

        if isinstance(cppreturn_statement.expression, function_call.FunctionCall):
            debug.line("validate_return_statement", f"cppreturn_statement.expression is a function call, assuming the function returns the correct type")
        else:
            mapping = self._conversion_data.funcsig_mapping_for_current_cfuncname()
            if mapping:
                cppfunc_return_type = mapping.cppfuncsig.return_type.type
                cpparg = arg_utils.to_cpparg(cppreturn_statement.expression, self._conversion_data)
                updated_cpp_expression = None

                debug.line("validate_return_statement", f"RETURN DEBUG: cppreturn_statement.expression=[{type(cppreturn_statement.expression)}]")

                if cpparg:
                    if cpparg.decl_spec.type != cppfunc_return_type:
                        updated_cpp_expression = literal.Literal(f"static_cast<GribStatus>({cpparg.name})")
                elif cppfunc_return_type == "GribStatus":
                    cpp_expression = cppreturn_statement.expression.as_string()
                    if cpp_expression == "0":
                        updated_cpp_expression = literal.Literal("GribStatus::SUCCESS")
                    elif not cpp_expression.startswith("GribStatus"):
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
        
        # Handle AccessorPtr types
        cpparg = self._conversion_data.cpparg_for_cppname(cppstruct_member_access.name)
        if cpparg and cpparg.decl_spec.type == "AccessorPtr":
            data_member = cppstruct_member_access.member
            if data_member:
                if data_member.name == "name":
                    data_member.name += "().get().c_str()" # It's read-only so this is ok!
                if data_member.name == "parent":
                    # For now we'll strip off the parent bit as that is accessing the grib_section, which we'll probably get
                    # another way!
                    updated_cppstruct_member_access = struct_member_access.StructMemberAccess(cppstruct_member_access.access,
                                                                                            cppstruct_member_access.name,
                                                                                            cppstruct_member_access.index)
                    debug.line("validate_struct_member_access", f"Updating AccessorPtr grib_section access: [{debug.as_debug_string(cppstruct_member_access)}]->[{updated_cppstruct_member_access}]")
                    return updated_cppstruct_member_access

        return super().validate_struct_member_access(cstruct_member_access, cppstruct_member_access)

    def validate_macro_instantiation(self, cmacro_instantiation, cppmacro_instantiation):

        if cppmacro_instantiation.name == "STR_EQUAL_NOCASE":
            updated_cppmacro_instantiation = macro_instantation.MacroInstantation("strcmpNoCase", cppmacro_instantiation.expression)
            debug.line("validate_macro_instantiation", f"Updated macro, from=[{debug.as_debug_string(cppmacro_instantiation)}] to=[{debug.as_debug_string(updated_cppmacro_instantiation)}]")
            return updated_cppmacro_instantiation

        return super().validate_macro_instantiation(cmacro_instantiation, cppmacro_instantiation)
