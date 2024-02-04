
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

# Pass this to the conversion_data object to be accessed by the conversion routines
class DefaultConversionValidation(conversion_validation.ConversionValidation):

    def validate_function_call(self, cfunction_call, cppfunction_call):

        # See if there is an existing mapping defined
        mapping = self._conversion_data.funcsig_mapping_for_cfuncname(cfunction_call.name)

        debug.line("validate_function_call", f"cfunction_call=[{debug.as_debug_string(cfunction_call)}] cppfunction_call=[{debug.as_debug_string(cppfunction_call)}] mapping=[{mapping}]")

        if mapping:
            debug.line("validate_function_call", f"mapping.cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] mapping.cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")
            cpp_args = []
            for arg_entry in mapping.cppfuncsig.args:
                if arg_entry != NONE_VALUE:
                    # The size of cpp_args should be the same as the next valid index :-)
                    cpp_arg_entry = self.validate_function_call_arg(cppfunction_call.args[len(cpp_args)], arg_entry)
                    cpp_args.append(cpp_arg_entry)
            return function_call.FunctionCall(cppfunction_call.name, cpp_args)
         
        if cfunction_call.name == "strcmp":
            return binary_operation.BinaryOperation(cppfunction_call.args[0], "==", cppfunction_call.args[1])
              
        if cfunction_call.name == "strlen":
            # Replace the function call with a StructMemberAccess representing the container.size() call...
            return self._container_utils.create_cpp_container_length_arg(cppfunction_call.args[0].name)

        return cppfunction_call

    # Check use of references when calling functions...
    def validate_function_call_arg(self, calling_arg_value, target_arg):

        if isinstance(calling_arg_value, unary_operation.UnaryOperation) and \
            calling_arg_value.unary_op.as_string() == "&":
                # For now, assume we don't call functions with addressof
                debug.line("validate_function_call_args", f"calling_arg_value=[{debug.as_debug_string(calling_arg_value)}] and target_arg=[{debug.as_debug_string(target_arg)}] - stripping first [&]")
                return unary_operation.UnaryOperation("", calling_arg_value.operand)

        # Just return the passed in value!
        return calling_arg_value

    def validate_unary_operation(self, cunary_operation, cppunary_operation):
        debug.line("validate_unary_operation", f"cppunary_operation.operand string=[{debug.as_debug_string(cppunary_operation.operand)}] type=[{type(cppunary_operation.operand).__name__}]")
        
        cpparg = arg_utils.to_cpparg(cppunary_operation.operand, self._conversion_data)
        debug.line("validate_unary_operation", f"cpparg=[{debug.as_debug_string(cpparg)}]")

        if cpparg:
            if self._conversion_data.is_container_type(cpparg.decl_spec.type):
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

        # If we've got a StructMemberAccess object with a .size(), then we need to tidy up!
        cppleft = cppbinary_operation.left_operand
        if isinstance(cppleft, struct_member_access.StructMemberAccess):
             if cppleft.member and cppleft.member.name == "size()":
                cppright = cppbinary_operation.right_operand
                if cppright.as_string() == "0":
                    cppleft.member.name = "clear()"
                else:
                    cppleft.member.name = f"resize({cppright.as_string()})"
                return cppleft

        return cppbinary_operation
