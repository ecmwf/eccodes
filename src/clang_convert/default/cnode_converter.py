
import debug
import default.cppcode as cppcode
import clang.cindex
import cnode_utils
import code_object_converter.arg_converter as arg_converter
import code_object.variable_declaration as variable_declaration
import code_object.code_lines as code_lines
import code_object.init_list as init_list
import code_object.declaration_specifier as declaration_specifier
import code_object.paren_expression as paren_expression
import code_object.unary_expression as unary_expression
import code_object.unary_operation as unary_operation
import code_object.binary_operation as binary_operation
import code_object.struct_member_access as struct_member_access
import code_object.function_call as function_call
import code_object.if_statement as if_statement
import code_object.for_statement as for_statement

# Convert a C node (AST) into lines of C++ code
#
# Override this class to specialise the conversion process...

class CNodeConverter:
    def __init__(self, manifest_class) -> None:
        self._manifest_class = manifest_class

    # Perform the conversion and return a list of lines
    # CNode can be a single node or a list of nodes (for example, the global declaration is a list)
    # A list will be treated as a single block of code
    def convert(self, cnode, transforms, macros):
        self._transforms = transforms
        self._macros = macros

        converted_lines = code_lines.CodeLines()

        if isinstance(cnode, list):
            for entry in cnode:
                converted_lines.add_lines(self.convert_node(entry))
        else:
            converted_lines.add_lines(self.convert_node(cnode))
        
        return converted_lines.as_lines()

    # Main entry point to convert any node
    # Fine to call recursively
    #
    # Note - Prefer to call this as it handles macro expansions
    def convert_node(self, node):

        # Handle macros
        macro_instantiation_node = self._macros.instantiation_node_for(node)
        if macro_instantiation_node:
            return self.convert_macro_instantiation(macro_instantiation_node)

        if node.kind.is_statement():
            return self.convert_STMT_node(node)
        elif node.kind.is_declaration():
            return self.convert_DECL_node(node)
        elif node.kind.is_expression():
            return self.convert_EXPR_node(node)
        elif node.kind.is_reference():
            return self.convert_REF_node(node)
        elif node.kind == clang.cindex.CursorKind.MACRO_DEFINITION:
            debug.line("convert_node", f"IGNORING MACRO spelling=[{node.spelling}] kind=[{node.kind}]")
            assert False
        else:
            assert False, f"Unclassified kind=[{node.kind}] spelling=[{node.spelling}]"

        return None
    
    # Default behaviour for node kinds that we ignore
    def convert_node_ignored_kind(self, node):
        debug.line("convert_node_ignored_kind", f"*** IGNORING *** spelling=[{node.spelling}] kind=[{node.kind}]")
        if node.kind == clang.cindex.CursorKind.UNEXPOSED_DECL:
            debug.line("convert_node_ignored_kind", f"     Not dumping node - could be a lot of tokens if #include !")
        else:
            cnode_utils.dump_node(node,5)

        return None

    # Placeholder for node kinds that we should support, but haven't yet implemented!
    def convert_node_not_implemented(self, node):
        debug.line("convert_node_not_implemented", f"*** kind=[{node.kind}] not implemented ***")
        cnode_utils.dump_node(node,5)
        debug.line("convert_node_not_implemented", f"No convert routine implemented for kind=[{node.kind}]")
        #assert False, f"No convert routine implemented for kind=[{node.kind}]"
        return None

    # =================================== Macros Convert functions [BEGIN] ===================================

    def convert_macro_instantiation(self, node):
        debug.line("convert_macro_instantiation", f"MACRO spelling=[{node.spelling}] kind=[{node.kind}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")
        token_text = "".join([token.spelling for token in node.get_tokens()]) + ";"
        debug.line("convert_macro_instantiation", f"MACRO token text=[{token_text}]")
        macro_lines = code_lines.CodeLines()
        macro_lines.add_line(token_text)

        return macro_lines

        '''debug.line("convert_macro_instantiation", f"MACRO INST spelling=[{node.spelling}] kind=[{node.kind}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")

        # Iterate children to find node with tokens and process...
        for child in node.get_children():
            tokens = child.get_tokens()
            token = next(tokens, None)
            if  token != None:
                debug.line("convert_macro_instantiation", f"MACRO INST [1] FOUND node with tokens: spelling=[{child.spelling}] kind=[{child.kind}] extent=[{child.extent.start.line}:{child.extent.start.column} -> {child.extent.end.line}:{child.extent.end.column}]")
                cnode_utils.dump_node(child, 2)
                debug.line("convert_macro_instantiation", f"MACRO INST [2] Converting node spelling=[{child.spelling}] kind=[{child.kind}] extent=[{child.extent.start.line}:{child.extent.start.column} -> {child.extent.end.line}:{child.extent.end.column}]")
                return self.convert_node(child)
            else:
                self.convert_macro_instantiation(child)

        return None'''

    # =================================== Macros Convert functions [END]   ===================================

    # =================================== convert_STMT_funcs [BEGIN] ===================================

    # Just iteratively call convert_node
    def convert_COMPOUND_STMT(self, node):

        debug.line("convert_COMPOUND_STMT", f">>> spelling=[{node.spelling}] kind=[{node.kind}]")
        cnode_utils.dump_node(node, 1)

        stmt_lines = code_lines.CodeLines()

        stmt_lines.add_line("{")

        for child in node.get_children():
            stmt_lines.add_lines(self.convert_node(child))

        stmt_lines.add_line("}")

        return stmt_lines

    # DECL_STMT is an "Adaptor class for mixing declarations with statements and expressions."
    # For now we'll just call convert_node recursively with the children...
    def convert_DECL_STMT(self, node):
        stmt_lines = code_lines.CodeLines()

        for child in node.get_children():
            debug.line("convert_DECL_STMT", f"child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")
            stmt_lines.add_lines(self.convert_node(child))

        return stmt_lines

    def convert_RETURN_STMT(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for return statement"
        return_value = children[0]

        tokens = [token.spelling for token in node.get_tokens()]
        return_value_tokens = [token.spelling for token in return_value.get_tokens()]

        if len(return_value_tokens) == 0:
            # Probably a macro - return as string
            debug.line("convert_RETURN_STMT", f"The return value has no tokens: using the top-level tokens=[{tokens[1:]}]")
            return_cvalue = code_lines.CodeLines(f"{' '.join(t for t in tokens[1:])}")
        else:
            return_cvalue = self.convert_node(return_value)

        return_lines = return_cvalue.as_lines()
        return_lines[0] = "return " + return_lines[0]
        return_lines[-1] += ";"

        return code_lines.CodeLines(return_lines)

    def convert_IF_STMT(self, node):
        debug.line("convert_IF_STMT", f"IF spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

        children = list(node.get_children())
        child_count = len(children)
        assert child_count >= 2, f"Expected at least two children for if statement"

        if_expression = self.convert_node(children[0])
        if_action = self.convert_node(children[1])
        if_stmt = if_statement.IfStatement(if_expression, if_action)

        if child_count == 3:
            else_statement = self.convert_node(children[2])
            if_stmt.add_else(else_statement)

        return if_stmt

    def convert_FOR_STMT(self, node):
        debug.line("convert_FOR_STMT", f"IF spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

        init_statement = condition = iteration_expression = statement = None

        # A for loop can have empty sections (e.g. for(i=0;;++i){} ) but we don't know which missing nodes
        # correspond with these, so need to identify them from the tokens...
        tokens = [token.spelling for token in node.get_tokens()]
            
        # Find the indices of the parens and semicolons in "for(;;)"
        loop_indices = [i for i, token in enumerate(tokens) if token in ['(',';',')']]
        
        # Check there are at least two semicolons as well as open/close parens
        assert len(loop_indices) >= 4, "Could not find 2 semicolons in for loop"

        open_paren_token_index = loop_indices[0]
        init_end_token_index = loop_indices[1]
        cond_end_token_index = loop_indices[2]
        close_paren_token_index = loop_indices[3]

        children = node.get_children()

        # init_statement
        if init_end_token_index-open_paren_token_index > 1:
            child = next(children, None)
            assert child, f"init_statement node=[None]: open_paren_token_index=[{open_paren_token_index}] init_end_token_index=[{init_end_token_index}]"
            init_statement = self.convert_node(child)

        # condition
        if cond_end_token_index-init_end_token_index > 1:
            child = next(children, None)
            assert child, f"condition node=[None]: init_end_token_index=[{init_end_token_index}] cond_end_token_index=[{cond_end_token_index}]"
            condition = self.convert_node(child)

        # iteration_expression
        if close_paren_token_index - cond_end_token_index > 1:
            child = next(children, None)
            assert child, f"iteration_expression node=[None]: cond_end_token_index=[{cond_end_token_index}] close_paren_token_index=[{close_paren_token_index}]"
            iteration_expression = self.convert_node(child)

        # statement
        child = next(children, None)
        assert child, f"For loop has no statement! node=[None]"
        statement = self.convert_node(child)

        for_stmt = for_statement.ForStatement(init_statement, condition, iteration_expression, statement)

        return for_stmt

    convert_STMT_funcs = {
        clang.cindex.CursorKind.COMPOUND_STMT:  convert_COMPOUND_STMT,
        clang.cindex.CursorKind.DECL_STMT:      convert_DECL_STMT,
        clang.cindex.CursorKind.CASE_STMT:      convert_node_not_implemented,
        clang.cindex.CursorKind.DEFAULT_STMT:   convert_node_not_implemented,
        clang.cindex.CursorKind.IF_STMT:        convert_IF_STMT,
        clang.cindex.CursorKind.SWITCH_STMT:    convert_node_not_implemented,
        clang.cindex.CursorKind.WHILE_STMT:     convert_node_not_implemented,
        clang.cindex.CursorKind.DO_STMT:        convert_node_not_implemented,
        clang.cindex.CursorKind.FOR_STMT:       convert_FOR_STMT,
        clang.cindex.CursorKind.GOTO_STMT:      convert_node_not_implemented,
        clang.cindex.CursorKind.CONTINUE_STMT:  convert_node_not_implemented,
        clang.cindex.CursorKind.BREAK_STMT:     convert_node_not_implemented,
        clang.cindex.CursorKind.RETURN_STMT:    convert_RETURN_STMT,
        clang.cindex.CursorKind.NULL_STMT:      convert_node_not_implemented,
    }
    
    def convert_STMT_node(self, node):
        debug.line("convert_STMT_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.convert_STMT_funcs.keys():
            return self.convert_STMT_funcs[node.kind](self, node)
        else:
            return self.convert_node_ignored_kind(node)

    # =================================== convert_STMT_funcs [END]   ===================================

    # =================================== convert_DECL_funcs [BEGIN] ===================================

    # Assume this is just a declaration - the function body will be converted via a function class...
    def convert_FUNCTION_DECL(self, node):
        cppfuncsig = self._transforms.cppfuncsig_for_cfuncname(node.spelling)
        if not cppfuncsig:
            cfuncsig = cnode_utils.create_cfuncsig(node)
            cfuncsig_conv = self._manifest_class.CFUNCSIG_CONVERTER(cfuncsig)
            mapping = cfuncsig_conv.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_funcsig_mappings(mapping)
            cppfuncsig = mapping.cppfuncsig
            assert cppfuncsig, f"Error creating cppfuncsig from node [{node.spelling}]"

        return cppfuncsig

    def convert_STRUCT_DECL(self, node):
        cstruct_arg = cnode_utils.create_cstruct_arg(node)

        #TODO - Convert to C++

        return cstruct_arg

    def convert_PARM_DECL(self, node):
        return None

    def convert_TYPEDEF_DECL(self, node):
        typedef_type = node.underlying_typedef_type

        if typedef_type.kind == clang.cindex.TypeKind.POINTER:
            pointee = typedef_type.get_pointee()

            if pointee.kind == clang.cindex.TypeKind.FUNCTIONPROTO:
                # It's a function pointer
                cfuncsig_pointer = cnode_utils.create_cfuncsig_pointer(node)
                cfuncsig_pointer_conv = self._manifest_class.CFUNCSIG_POINTER_CONVERTER(cfuncsig_pointer)
                mapping = cfuncsig_pointer_conv.create_funcsig_mapping(self._transforms)
                self._transforms.add_to_funcsig_pointer_mappings(mapping)
                cppfuncsig_pointer = mapping.cppfuncsig
                assert cppfuncsig_pointer, f"Error creating cppfuncsig_pointer from node [{node.spelling}]"
                
                debug.line("convert_TYPEDEF_DECL", f"cfuncsig_pointer   = [{cfuncsig_pointer.as_string()}]")
                debug.line("convert_TYPEDEF_DECL", f"cppfuncsig_pointer = [{cppfuncsig_pointer.as_string()}]")
                return cppfuncsig_pointer
            
        elif typedef_type.kind == clang.cindex.TypeKind.ELABORATED:
            # We don't need to process e.g. "typedef struct proj_mapping proj_mapping;" as C++ doesn't need it!
            pass

        debug.line("convert_TYPEDEF_DECL", f"Ignoring node spelling=[{node.spelling}] typedef_type.kind=[{typedef_type.kind}]")
        return None

    def convert_FIELD_DECL(self, node):
        carg = cnode_utils.create_carg(node)
        arg_conv = arg_converter.ArgConverter(carg)
        cpparg = arg_conv.to_cpp_arg(self._transforms)
        
        debug.line("convert_FIELD_DECL", f"Converted spelling=[{node.spelling}] to cpparg=[{cpparg.as_string()}]")
        return cpparg

    def convert_VAR_DECL(self, node):
        cvariable = cnode_utils.create_carg(node)
        cvalue = None
        for child in node.get_children():
            if child.kind.is_expression():
                cvalue = self.convert_node(child)                
            else:
                debug.line("convert_VAR_DECL", f"Ignoring child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")
        
        return variable_declaration.VariableDeclaration(cvariable, cvalue)

    convert_DECL_funcs = {
        clang.cindex.CursorKind.FUNCTION_DECL:      convert_FUNCTION_DECL,
        clang.cindex.CursorKind.STRUCT_DECL:        convert_STRUCT_DECL,
        clang.cindex.CursorKind.UNION_DECL:         convert_node_not_implemented,
        clang.cindex.CursorKind.FIELD_DECL:         convert_FIELD_DECL,
        clang.cindex.CursorKind.VAR_DECL:           convert_VAR_DECL,
        clang.cindex.CursorKind.PARM_DECL:          convert_PARM_DECL,
        clang.cindex.CursorKind.TYPEDEF_DECL:       convert_TYPEDEF_DECL,
        clang.cindex.CursorKind.TYPE_ALIAS_DECL:    convert_node_not_implemented,
    }

    def convert_DECL_node(self, node):
        debug.line("convert_DECL_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.convert_DECL_funcs.keys():
            return self.convert_DECL_funcs[node.kind](self, node)
        else:
            return self.convert_node_ignored_kind(node)

    # =================================== convert_DECL_funcs [END]   ===================================

    # =================================== convert_EXPR_funcs [BEGIN] ===================================

    # We just pass the request straight to the child...
    def convert_UNEXPOSED_EXPR(self, node):
        # There should be a single child node...
        child = list(node.get_children())[0]

        debug.line("convert_UNEXPOSED_EXPR", f"Passing to child: spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return self.convert_node(child)

    # This is a recursive convertion...
    def convert_INIT_LIST_EXPR(self, node):
        #return cnode_utils.create_cinit_list(node)
        debug.line("convert_INIT_LIST_EXPR", f"spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")
        init_list_decl_spec = declaration_specifier.DeclSpec.from_decl_specifier_seq(node.type.spelling)
        cinit_list = init_list.InitList(init_list_decl_spec)

        for child in node.get_children():
            cvalue = self.convert_node(child)
            if cvalue:
                cinit_list.add_entry(cvalue)

        return cinit_list

    def convert_INTEGER_LITERAL(self, node):
        if node.spelling:
            debug.line("convert_INTEGER_LITERAL", f"Got value from spelling=[{node.spelling}]")
            return code_lines.CodeLines(node.spelling)
        
        # We'll have to extract the value from the tokens
        tokens=[token.spelling for token in node.get_tokens()]
        if tokens:
            debug.line("convert_INTEGER_LITERAL", f"Got value from tokens=[{tokens[0]}]")
            return code_lines.CodeLines(tokens[0])
        
        assert False, f"Could not extract integer literal"

    def convert_STRING_LITERAL(self, node):
        return code_lines.CodeLines(node.spelling)

    def convert_PAREN_EXPR(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for paren expression"
        expression = children[0]

        expression_value = self.convert_node(expression)
        c_paren_expr = paren_expression.ParenExpression(expression_value)
        return c_paren_expr

    def convert_CXX_UNARY_EXPR(self, node):
        keyword = node.spelling
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for unary expression"
        expression = children[0]

        expression_value = self.convert_node(expression)
        c_unary_expr = unary_expression.UnaryExpression(keyword, expression_value)
        return c_unary_expr

    def convert_UNARY_OPERATOR(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for unary operator"
        right_operand = children[0]

        # Tokenize and find the operator
        tokens = [token.spelling for token in node.get_tokens()]
        right_tokens = [token.spelling for token in right_operand.get_tokens()]

        # Find the operator by excluding operand tokens
        operator_tokens = [t for t in tokens if t not in right_tokens]
        debug.line("convert_UNARY_OPERATOR", f"Unary Operator = [{operator_tokens}]")
        debug.line("convert_UNARY_OPERATOR", f"right_operand...")
        cnode_utils.dump_node(right_operand)

        operator_str = "".join(t for t in operator_tokens)
        right_operand_cvalue = self.convert_node(right_operand)

        c_unary_op = unary_operation.UnaryOperation(operator_str, right_operand_cvalue)
        return c_unary_op

    def convert_BINARY_OPERATOR(self, node):
        children = list(node.get_children())
        assert len(children) == 2, f"Expected exactly two children for binary operator"
        left_operand, right_operand = children

        # Tokenize and find the operator
        tokens = [token.spelling for token in node.get_tokens()]
        left_tokens = [token.spelling for token in left_operand.get_tokens()]
        right_tokens = [token.spelling for token in right_operand.get_tokens()]

        # Find the operator by excluding operand tokens
        tokens_count = len(tokens)
        left_tokens_count = len(left_tokens)
        if left_tokens_count >= tokens_count:
            debug.line("convert_BINARY_OPERATOR", f"operator_tokens is empty!")
            debug.line("convert_BINARY_OPERATOR", f" -> tokens       = [{tokens}]")
            debug.line("convert_BINARY_OPERATOR", f" -> left_tokens  = [{left_tokens}]")
            debug.line("convert_BINARY_OPERATOR", f" -> right_tokens = [{right_tokens}]")
            assert False, "Binary operator: operator_tokens array should not be empty!"

        #operator_tokens = [t for t in tokens if t not in left_tokens and t not in right_tokens]
        operator_tokens = tokens[left_tokens_count:]

        operator_token = operator_tokens[0]
        debug.line("convert_BINARY_OPERATOR", f"operator_token=[{operator_token}] => operator_tokens=[{operator_tokens}]")

        left_operand_cvalue = self.convert_node(left_operand)

        if len(operator_tokens) > 1:
            # The top level tokens don't match the right_operand tokens. This can happen if the top-level
            # contains (for example) a macro definition. 
            # For now we'll just take the top-level tokens as a string
            debug.line("convert_BINARY_OPERATOR", f"The top level tokens don't match the right_operand tokens: using the top-level tokens=[{operator_tokens[1:]}]")
            right_operand_cvalue = code_lines.CodeLines(f"{' '.join(t for t in operator_tokens[1:])}")
        else:
            right_operand_cvalue = self.convert_node(right_operand)

        c_binary_op = binary_operation.BinaryOperation(left_operand_cvalue, operator_token, right_operand_cvalue)
        return c_binary_op

    def convert_COMPOUND_ASSIGNMENT_OPERATOR(self, node):
        debug.line("convert_COMPOUND_ASSIGNMENT_OPERATOR", f"Forwarding to convert_BINARY_OPERATOR spelling=[{node.spelling}] type=[{node.type.spelling}]")
        return self.convert_BINARY_OPERATOR(node)

    def convert_DECL_REF_EXPR(self, node):
        return code_lines.CodeLines(node.spelling)

    # This is simplified for now, need to come back to...
    def convert_MEMBER_REF_EXPR(self, node):
        member_name = node.spelling
        cstruct_member_access = struct_member_access.StructMemberAccess(None, member_name, None)

        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                parent_name = child.spelling
                cparent_struct_member_access = struct_member_access.StructMemberAccess(None, parent_name, None)
                cstruct_member_access.access = "->"
                cparent_struct_member_access.member = cstruct_member_access
                return cparent_struct_member_access
            
            debug.line("convert_MEMBER_REF_EXPR", f"*** IGNORING *** child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return None

    def convert_CALL_EXPR(self, node):
        cfunc_call = function_call.FunctionCall(node.spelling)

        for arg_node in node.get_arguments():
            arg_entry = self.convert_node(arg_node)
            debug.line("convert_CALL_EXPR", f"arg_node spelling=[{arg_node.spelling}] type=[{arg_node.type.spelling}] kind=[{arg_node.kind}]")
            if arg_entry:
                debug.line("convert_CALL_EXPR", f"arg_node arg_entry=[{arg_entry.as_string()}]")
                cfunc_call.add_arg(arg_entry.as_string())
            else:
                debug.line("convert_CALL_EXPR", f"arg_node arg_entry=[{arg_entry}] - IS THIS AN ERROR?")

        return cfunc_call

    def convert_CSTYLE_CAST_EXPR(self, node):
        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                # We ignore the cast, and just return the object
                return code_lines.CodeLines(child.spelling)
            
            debug.line("convert_CSTYLE_CAST_EXPR", f"*** IGNORING *** child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return None

    convert_EXPR_funcs = {
        clang.cindex.CursorKind.UNEXPOSED_EXPR:                 convert_UNEXPOSED_EXPR,
        clang.cindex.CursorKind.DECL_REF_EXPR:                  convert_DECL_REF_EXPR,
        clang.cindex.CursorKind.MEMBER_REF_EXPR:                convert_MEMBER_REF_EXPR,
        clang.cindex.CursorKind.CALL_EXPR:                      convert_CALL_EXPR,
        clang.cindex.CursorKind.ARRAY_SUBSCRIPT_EXPR:           convert_node_not_implemented,
        clang.cindex.CursorKind.CSTYLE_CAST_EXPR:               convert_CSTYLE_CAST_EXPR,
        clang.cindex.CursorKind.COMPOUND_LITERAL_EXPR:          convert_node_not_implemented,
        clang.cindex.CursorKind.INIT_LIST_EXPR:                 convert_INIT_LIST_EXPR,
        clang.cindex.CursorKind.INTEGER_LITERAL:                convert_INTEGER_LITERAL,
        clang.cindex.CursorKind.FLOATING_LITERAL:               convert_node_not_implemented,
        clang.cindex.CursorKind.STRING_LITERAL:                 convert_STRING_LITERAL,
        clang.cindex.CursorKind.CHARACTER_LITERAL:              convert_node_not_implemented,
        clang.cindex.CursorKind.PAREN_EXPR:                     convert_PAREN_EXPR,
        clang.cindex.CursorKind.CXX_UNARY_EXPR:                 convert_CXX_UNARY_EXPR,
        clang.cindex.CursorKind.UNARY_OPERATOR:                 convert_UNARY_OPERATOR,
        clang.cindex.CursorKind.BINARY_OPERATOR:                convert_BINARY_OPERATOR,
        clang.cindex.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR:   convert_COMPOUND_ASSIGNMENT_OPERATOR,
        clang.cindex.CursorKind.CONDITIONAL_OPERATOR:           convert_node_not_implemented,
    }

    def convert_EXPR_node(self, node):
        debug.line("convert_EXPR_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.convert_EXPR_funcs.keys():
            return self.convert_EXPR_funcs[node.kind](self, node)
        else:
            return self.convert_node_ignored_kind(node)

    # =================================== convert_EXPR_funcs [END]   ===================================

    # =================================== convert_REF_funcs [BEGIN]  ===================================

    def convert_TYPE_REF(self, node):
        return None

    convert_REF_funcs = {
        clang.cindex.CursorKind.TYPE_REF:       convert_TYPE_REF,
        clang.cindex.CursorKind.TEMPLATE_REF:   convert_node_not_implemented,
        clang.cindex.CursorKind.MEMBER_REF:     convert_node_not_implemented,
    }

    def convert_REF_node(self, node):
        debug.line("convert_REF_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.convert_REF_funcs.keys():
            return self.convert_REF_funcs[node.kind](self, node)
        else:
            return self.convert_node_ignored_kind(node)

    # =================================== convert_REF_funcs [END]    ===================================

