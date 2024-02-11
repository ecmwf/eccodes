
import utils.debug as debug
import code_object_converter.conversion_pack.conversion_validation as conversion_validation
import code_object.unary_operation as unary_operation
from code_object.code_interface import NONE_VALUE
import code_object.function_call as function_call
import code_object.literal as literal
import re
import code_object.binary_operation as binary_operation
import code_object.struct_member_access as struct_member_access
import code_object_converter.conversion_pack.arg_utils as arg_utils
import code_object.variable_declaration as variable_declaration
import code_object.paren_expression as paren_expression
from code_object_converter.conversion_funcs import as_commented_out_code
from utils.string_funcs import is_number

# Pass this to the conversion_data object to be accessed by the conversion routines
class DefaultConversionValidation(conversion_validation.ConversionValidation):

    def validate_function_call(self, cfunction_call, cppfunction_call):

        # See if there is an existing mapping defined
        mapping = self._conversion_data.funcsig_mapping_for_cfuncname(cfunction_call.name)

        debug.line("validate_function_call", f"cfunction_call=[{debug.as_debug_string(cfunction_call)}] cppfunction_call=[{debug.as_debug_string(cppfunction_call)}] mapping=[{mapping}]")

        if mapping:
            return self.validate_function_call_args(cppfunction_call, mapping.cppfuncsig)
         
        if cfunction_call.name == "strcmp":
            return binary_operation.BinaryOperation(cppfunction_call.args[0], "==", cppfunction_call.args[1])

        if cfunction_call.name == "strcpy":
            return binary_operation.BinaryOperation(cppfunction_call.args[0], "=", cppfunction_call.args[1])

        if cfunction_call.name == "strcat":
            return binary_operation.BinaryOperation(cppfunction_call.args[0], "+=", cppfunction_call.args[1])

        if cfunction_call.name == "strlen":
            # Replace the function call with a StructMemberAccess representing the container.size() call...
            cpparg = arg_utils.to_cpparg(cppfunction_call.args[0], self._conversion_data)
            assert cpparg
            return self._container_utils.create_cpp_container_length_arg(cpparg.name)

        return cppfunction_call

    # Confirm the args are correct for the target type
    # Returns an updated cppfunction_call
    #
    # NOTE: This function assumes all NONE_VALUE args have already been removed from the C++ call!
    def validate_function_call_args(self, cppfunction_call, target_cppfuncsig):
        debug.line("validate_function_call_args", f"cppfunction_call=[{debug.as_debug_string(cppfunction_call)}] target_cppfuncsig=[{debug.as_debug_string(target_cppfuncsig)}]")
        cpp_args = []
        
        arg_index = 0
        for arg_entry in target_cppfuncsig.args:
            if arg_entry != NONE_VALUE:
                cpp_arg_entry = self.validate_function_call_arg(cppfunction_call.args[arg_index], arg_entry)
                cpp_args.append(cpp_arg_entry)
                arg_index += 1

        return function_call.FunctionCall(cppfunction_call.name, cpp_args)

    # Check use of references when calling functions...
    def validate_function_call_arg(self, calling_arg_value, target_arg):

        if isinstance(calling_arg_value, unary_operation.UnaryOperation) and \
            calling_arg_value.unary_op.as_string() == "&":
                # For now, assume we don't call functions with addressof
                debug.line("validate_function_call_args", f"calling_arg_value=[{debug.as_debug_string(calling_arg_value)}] and target_arg=[{debug.as_debug_string(target_arg)}] - stripping first [&]")
                return unary_operation.UnaryOperation("", calling_arg_value.operand)

        # Just return the passed in value!
        return calling_arg_value

    def validate_unary_expression(self, cunary_expression, cppunary_expression):
        debug.line("validate_unary_operation", f"cppunary_expression.keyword=[{debug.as_debug_string(cppunary_expression.keyword)}] cppunary_expression.expression=[{debug.as_debug_string(cppunary_expression.expression)}]")
        
        if cppunary_expression.keyword == "sizeof":
            assert isinstance(cppunary_expression.expression, paren_expression.ParenExpression)
            cpparg = arg_utils.to_cpparg(cppunary_expression.expression, self._conversion_data)
            if cpparg and self._conversion_data.is_container_type(cpparg.decl_spec.type):
                cpp_size_call = literal.Literal(f"{cpparg.name}.size()")
                debug.line("validate_unary_expression", f"Updating size evaluation: [{debug.as_debug_string(cppunary_expression)}]->[{debug.as_debug_string(cpp_size_call)}]")
                return cpp_size_call

        return cppunary_expression

    def validate_unary_operation(self, cunary_operation, cppunary_operation):
        debug.line("validate_unary_operation", f"cppunary_operation.operand string=[{debug.as_debug_string(cppunary_operation.operand)}] type=[{type(cppunary_operation.operand).__name__}]")
        
        cpparg = arg_utils.to_cpparg(cppunary_operation.operand, self._conversion_data)
        debug.line("validate_unary_operation", f"cpparg=[{debug.as_debug_string(cpparg)}]")

        if cpparg:
            if self._conversion_data.is_container_type(cpparg.decl_spec.type):
                if cppunary_operation.unary_op.value == "!":
                    cpp_empty_call = literal.Literal(f"{cpparg.name}.empty()")
                    debug.line("validate_unary_operation", f"Updating empty evaluation: [{debug.as_debug_string(cppunary_operation)}]->[{debug.as_debug_string(cpp_empty_call)}]")
                    return cpp_empty_call
                else:
                    # C++ variable is a container, so we'll strip the *
                    debug.line("validate_unary_operation", f"Stripping [*] from cppunary_operation=[{debug.as_debug_string(cppunary_operation)}]")
                    return cppunary_operation.operand
            elif cpparg.decl_spec.pointer == "&":
                debug.line("validate_unary_operation", f"Stripping [*] from ref type: current cppunary_operation=[{debug.as_debug_string(cppunary_operation)}]")
                return cppunary_operation.operand

        return cppunary_operation

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):
        # Check for the sizeof(x)/sizeof(*x) idiom (in the C code), and if x is a container in C++, replace with x.size()
        # Note we also check for sizeof(x)/sizeof(x[0])
        cvalue = cbinary_operation.as_string()
        m = re.search(rf"sizeof\(([^\)]+)\)\s*/\s*sizeof\((\*\1\)|(\1\[0\]\)))", cvalue)
        if m:
            cname = m.group(1)
            cpp_container_arg = self._container_utils.cname_to_cpp_container_length(cname, self._conversion_data)
            debug.line("validate_binary_operation", f"sizeof(x)/sizeof(*x) x=[{cname}] cpp_container_arg=[{debug.as_debug_string(cpp_container_arg)}]")
            if cpp_container_arg:
                return cpp_container_arg

        # If we've updated a strcmp function call, we need to remove the return value comparison
        if cbinary_operation.left_operand.as_string().startswith("strcmp") and \
            not cppbinary_operation.left_operand.as_string().startswith("strcmp"):
                debug.line("validate_binary_operation", f"Removed strcmp return value comparison [{debug.as_debug_string(cppbinary_operation.binary_op)}][{debug.as_debug_string(cppbinary_operation.right_operand)}]")
             
                return cppbinary_operation.left_operand

        # If we've got a StructMemberAccess object, we're probably dealing with a container...
        cppleft = cppbinary_operation.left_operand
        cppbinary_op = cppbinary_operation.binary_op
        cppright = cppbinary_operation.right_operand

        if cppbinary_op.is_assignment():
            if isinstance(cppleft, struct_member_access.StructMemberAccess):
                cpparg = self._conversion_data.cpparg_for_cppname(cppleft.name)
                if cppleft.member:
                    if cppleft.member.name == "size()":
                        if cppright.as_string() == "0":
                            cppleft.member.name = "clear()"
                        else:
                            cppleft.member.name = f"resize({cppright.as_string()})"

                        # Double-check it's not const!
                        debug.line("validate_binary_operation", f"Performing const check cppleft.name=[{debug.as_debug_string(cppleft.name)}] cpparg=[{debug.as_debug_string(cpparg)}]")
                        if cpparg and cpparg.decl_spec.const_qualifier == "const":
                                return as_commented_out_code(cppleft, "Removing - conversion requires mutable operation on const type")
                        
                        return cppleft
                
                # See if we're assigning a single value to a container type
                if cpparg and self._conversion_data.is_container_type(cpparg.decl_spec.type):
                    cppright_value = cppright.as_string()

                    if is_number(cppright_value) or not self.is_cppfunction_returning_container(cppright):
                        cppleft.index = "[0]"
                        debug.line("validate_binary_operation", f"Assigning number to container, so updating it to access first element: cppleft=[{debug.as_debug_string(cppleft)}] cppright_value=[{cppright_value}]")
                        return binary_operation.BinaryOperation(cppleft, cppbinary_op, cppright)

                # Check if we're assigning to a data member and need to ensure the function is non-const
                data_member_name = cppleft.name
                debug.line("validate_binary_operation", f"CHECK is_cppdata_member({data_member_name})")
                    
                if self._conversion_data.is_cppdata_member(data_member_name):
                    self._conversion_data.set_current_cfuncname_non_const()

        elif cppbinary_op.is_comparison():
            cpparg = arg_utils.to_cpparg(cppleft, self._conversion_data)
            if cpparg and self._conversion_data.is_container_type(cpparg.decl_spec.type):
                cppleft = literal.Literal(f"{cpparg.name}.size()")
                return binary_operation.BinaryOperation(cppleft, cppbinary_op, cppright)

        return cppbinary_operation

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        cppvariable = cppvariable_declaration.variable

        if self._conversion_data.is_container_type(cppvariable.decl_spec.type):
            cppvalue = cppvariable_declaration.value
            cvariable = cvariable_declaration.variable
            if cppvalue.as_string() == "{}" and cvariable.decl_spec.is_array_type():
                # We need to give the container an initial capacity!
                # The consistent constructor is CONTAINER(SIZE, T) where T is a default value
                cppcontainer_constructor_call = literal.Literal(f"{cppvariable.as_string()}({cvariable.decl_spec.array_size()},{{}});")
                debug.line("validate_variable_declaration", f"Creating cppvariable=[{debug.as_debug_string(cppvariable)}] with initial size=[{cvariable.decl_spec.array_size()}] cppcontainer_constructor_call=[{debug.as_debug_string(cppcontainer_constructor_call)}]")
                return cppcontainer_constructor_call
            if cppvalue.as_string() == "NULL":
                # Init as {}
                debug.line("validate_variable_declaration", f"Changing NULL initialiser for container cppvariable=[{debug.as_debug_string(cppvariable)}] to [= {{}}")
                cppvalue = literal.Literal("{}")
                return variable_declaration.VariableDeclaration(cppvariable, cppvalue)

        return cppvariable_declaration

    # Helper to determine how a container should receive the value returned by the function
    # Returns True if the function returns e.g. std::vector<int> instead of int
    # Override as required...
    def is_cppfunction_returning_container(self, cppfunc_object):
        cppname = arg_utils.extract_name(cppfunc_object)
        if cppname:
            cppfuncsig = self._conversion_data.cppfuncsig_for_cppfuncname(cppname)
            if cppfuncsig and self._conversion_data.is_container_type(cppfuncsig.return_type.type):
                return True
        
        return False