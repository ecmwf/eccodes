
import utils.debug as debug
import code_object_converter.conversion_pack.conversion_validation as conversion_validation
import code_object.unary_operation as unary_operation
from code_object.code_interface import NONE_VALUE
import code_object.function_call as function_call
import code_object.literal as literal
import re
import code_object.binary_operation as binary_operation

# Pass this to the conversion_data object to be accessed by the conversion routines
class DefaultConversionValidation(conversion_validation.ConversionValidation):

    def validate_function_call(self, cfunction_call, cppfunction_call, callee_funcsig_mapping):
        if callee_funcsig_mapping:
            cpp_args = []
            for arg_entry in callee_funcsig_mapping.cppfuncsig.args:
                if arg_entry != NONE_VALUE:
                    # The size of cpp_args should be the same as the next valid index :-)
                    cpp_arg_entry = self.validate_function_call_arg(cppfunction_call.args[len(cpp_args)], arg_entry)
                    cpp_args.append(cpp_arg_entry)
            return function_call.FunctionCall(cppfunction_call.name, cpp_args)
         
        if cfunction_call.name == "strcmp":
            return binary_operation.BinaryOperation(cppfunction_call.args[0], "==", cppfunction_call.args[1])
              
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

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):
        # Check for the sizeof(x)/sizeof(*x) idiom (in the C code), and if x is a container in C++, replace with x.size()
        # Note we also check for sizeof(x)/sizeof(x[0])
        cvalue = cbinary_operation.as_string()
        m = re.search(rf"sizeof\(([^\)]+)\)\s*/\s*sizeof\((\*\1\)|(\1\[0\]\)))", cvalue)
        if m:
             cvariable = m.group(1)
             cpparg = self._conversion_data.funcbody_cpparg_for_carg_name(cvariable)
             if cpparg and self.is_cpp_container_type(cpparg.decl_spec):
                  return literal.Literal(f"{cpparg.name}.size()")

        # If we've updated a strcmp function call, we need to remove the return value comparison
        if cbinary_operation.left_operand.as_string().startswith("strcmp") and \
            not cppbinary_operation.left_operand.as_string().startswith("strcmp"):
                debug.line("validate_binary_operation", f"Removed strcmp return value comparison [{debug.as_debug_string(cppbinary_operation.binary_op)}][{debug.as_debug_string(cppbinary_operation.right_operand)}]")
             
                return cppbinary_operation.left_operand

        return cppbinary_operation

    # Returns True is the name is a pointer to a class instance
    def is_pointer_to_class_instance(self, arg_name):
         return arg_name in ["self", "this"]
    
    def is_cpp_container_type(self, cppdecl_spec):
        for type in [
            "std::string",
            "std::array",
            "std::vector",
            "std::map",
        ]:
             if type in cppdecl_spec.type:
                  return True

        return False
