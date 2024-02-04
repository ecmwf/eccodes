
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
    def validate_arg(self, carg, cpparg):
        return cpparg
    
    def validate_array_access(self, carray_access, cpparray_access):
        return cpparray_access

    def validate_binary_operation(self, cbinary_operation, cppbinary_operation):
        return cppbinary_operation
    
    def validate_code_objects(self, ccode_objects, cppcode_objects):
        return cppcode_objects

    def validate_compound_statement(self, ccompound_statement, cppcompound_statement):
        return cppcompound_statement

    def validate_conditional_operation(self, cconditional_operation, cppconditional_operation):
        return cppconditional_operation
    
    def validate_constructor_function(self, cconstructor_function, cppconstructor_function):
        return cppconstructor_function
    
    def validate_data_member(self, cdata_member, cppdata_member):
        return cppdata_member
    
    def validate_declaration_specifier(self, cdeclaration_specifier, cppdeclaration_specifier):
        return cppdeclaration_specifier
    
    def validate_destructor_function(self, cdestructor_function, cppdestructor_function):
        return cppdestructor_function

    def validate_for_statement(self, cfor_statement, cppfor_statement):
        return cppfor_statement

    def validate_funcsig(self, cfuncsig, cppfuncsig):
        return cppfuncsig

    def validate_funcsig_pointer(self, cfuncsig_pointer, cppfuncsig_pointer):
        return cppfuncsig_pointer

    def validate_function_call(self, cfunction_call, cppfunction_call, callee_funcsig_mapping):
        return cppfunction_call

    def validate_function(self, cfunction, cppfunction):
        return cppfunction

    def validate_global_function(self, cglobal_function, cppglobal_function):
        return cppglobal_function

    def validate_if_statement(self, cif_statement, cppif_statement):
        return cppif_statement
    
    def validate_init_list(self, cinit_list, cppinit_list):
        return cppinit_list

    def validate_literal(self, cliteral, cppliteral):
        return cppliteral

    def validate_macro_definition(self, cmacro_definition, cppmacro_definition):
        return cppmacro_definition

    def validate_macro_instantiation(self, cmacro_instantiation, cppmacro_instantiation):
        return cppmacro_instantiation
    
    def validate_member_function(self, cmember_function, cppmember_function):
        return cppmember_function

    def validate_operation(self, coperation, cppoperation):
        return cppoperation

    def validate_paren_expression(self, cparen_expression, cppparen_expression):
        return cppparen_expression

    def validate_return_statement(self, creturn_statement, cppreturn_statement):
        return cppreturn_statement

    def validate_struct_arg(self, cstruct_arg, cppstruct_arg):
        return cppstruct_arg

    def validate_struct_member_access(self, cstruct_member_access, cppstruct_member_access):
        return cppstruct_member_access

    def validate_unary_expression(self, cunary_expression, cppunary_expression):
        return cppunary_expression

    def validate_unary_operation(self, cunary_operation, cppunary_operation):
        return cppunary_operation

    def validate_value_declaration_reference(self, cvalue_declaration_reference, cppvalue_declaration_reference):
        return cppvalue_declaration_reference

    def validate_variable_declaration(self, cvariable_declaration, cppvariable_declaration):
        return cppvariable_declaration

    def validate_virtual_member_function(self, cvirtual_member_function, cppvirtual_member_function):
        return cppvirtual_member_function

    def validate_while_statement(self, cwhile_statement, cppwhile_statement):
        return cppwhile_statement
    
    # ---------- CodeObject Validation : End   -----------------------------------------------------
   