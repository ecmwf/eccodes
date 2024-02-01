
import utils.debug as debug

# Provides functions to perform extra validation as part of a conversion operation
#
# Holds a reference to the conversion_data (part of the containing conversion_pack class)
# This is the interface, override as required...
class ConversionValidation:
    def __init__(self, conversion_data):
        self._conversion_data = conversion_data

    # ---------- CodeObject Validation : Begin -----------------------------------------------------
    # Perform full validation of converted C++ function call
    # The callee mapping is provided to check e.g. the function call is using the correct references etc
    def validate_function_call(self, cfunction_call, cppfunction_call, callee_funcsig_mapping):
        return cppfunction_call

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        return cppvariable_declaration

    def validate_unary_operation(self, cunary_operation, cppunary_operation):
        return cppunary_operation

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):
        return cppbinary_operation
    
    def validate_if_statement(self, cif_statement, cppif_statement):
        return cppif_statement
    
    def validate_macro_instantiation(self, cmacro_instantiation, cppmacro_instantiation):
        return cppmacro_instantiation
    
    # ---------- CodeObject Validation : End   -----------------------------------------------------
   
