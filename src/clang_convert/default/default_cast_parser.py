
import utils.debug as debug
import clang.cindex
import utils.cnode_utils as cnode_utils
import code_object.code_objects as code_objects
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
import code_object.array_access as array_access
import code_object.conditional_operation as conditional_operation
import code_object.compound_statement as compound_statement

# Parse C AST nodes and create code interface objects (classes that implement the CodeInterface)
#
# These C code interface objects can then be fed into code interface converters to output C++ code
#
# The input C code is from AST nodes generated by Libclang
class DefaultCASTParser:
    def __init__(self) -> None:
        pass
    
    # Parse the AST and return a list of CodeInterface objects representing the C objects
    # CNode can be a single node or a list of nodes (for example, the global declaration is a list)
    # - A list will be treated as a single block of code
    def to_ccode_objects(self, cnode, conversion_data, macros):
        self._conversion_data = conversion_data
        self._macros = macros

        # Local override - resets the current debug state at function exit!
        include_ast_parser_debugging = False
        global_debug_enabled = debug.debug_enabled

        if global_debug_enabled and not include_ast_parser_debugging:
            debug.disable_debug()

        ccode_objects = code_objects.CodeObjects()

        if isinstance(cnode, list):
            for entry in cnode:
                cnode_code_object = self.parse_ast_node(entry)
                if cnode_code_object:
                    ccode_objects.add_code_object(cnode_code_object)
                else:
                    debug.line("to_ccode_objects", f"Code object is None for node spelling=[{entry.spelling}] type=[{entry.type.spelling}] kind=[{entry.kind}]")
        else:
            cnode_code_object = self.parse_ast_node(cnode)
            if cnode_code_object:
                ccode_objects.add_code_object(cnode_code_object)
            else:
                debug.line("to_ccode_objects", f"Code object is None for node spelling=[{cnode.spelling}] type=[{cnode.type.spelling}] kind=[{cnode.kind}]")
        
        if global_debug_enabled and not include_ast_parser_debugging:
            debug.enable_debug()

        return ccode_objects

    # Main entry point to parse an AST node and return a CodeInterface objects
    #
    # Can be called recursively
    #
    # Note - Prefer to call this as it handles macro expansions
    def parse_ast_node(self, node):

        # Handle macros
        macro_instantiation_node = self._macros.instantiation_node_for(node)
        if macro_instantiation_node:
            return self.parse_macro_instantiation(macro_instantiation_node)

        if node.kind.is_statement():
            return self.parse_STMT_node(node)
        elif node.kind.is_declaration():
            return self.parse_DECL_node(node)
        elif node.kind.is_expression():
            return self.parse_EXPR_node(node)
        elif node.kind.is_reference():
            return self.parse_REF_node(node)
        elif node.kind == clang.cindex.CursorKind.MACRO_DEFINITION:
            return self.parse_macro_definition(node)
        else:
            assert False, f"Unclassified kind=[{node.kind}] spelling=[{node.spelling}]"

        return None
    
    # Default behaviour for node kinds that we ignore
    def parse_node_ignored_kind(self, node):
        debug.line("parse_node_ignored_kind", f"*** IGNORING *** spelling=[{node.spelling}] kind=[{node.kind}]")
        '''if node.kind == clang.cindex.CursorKind.UNEXPOSED_DECL:
            debug.line("parse_node_ignored_kind", f"     Not dumping node - could be a lot of tokens if #include !")
        else:
            cnode_utils.dump_node(node,5)'''

        return None

    # Placeholder for node kinds that we should support, but haven't yet implemented!
    def parse_node_not_implemented(self, node):
        debug.line("parse_node_not_implemented", f"*** kind=[{node.kind}] not implemented ***")
        #cnode_utils.dump_node(node,5)
        debug.line("parse_node_not_implemented", f"No convert routine implemented for kind=[{node.kind}]")
        assert False, f"No convert routine implemented for kind=[{node.kind}]"
        return None

    # =================================== Macros Convert functions [BEGIN] ===================================

    def parse_macro_definition(self, node):
        debug.line("parse_macro_definition", f"MACRO spelling=[{node.spelling}] kind=[{node.kind}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")
        tokens = [token.spelling for token in node.get_tokens()]
        debug.line("parse_macro_definition", f"MACRO tokens=[{tokens}]")
        tokens_count = len(tokens)
        assert tokens_count > 0, f"Expected at least 1 macro token!"

        macro_text = "#define " + tokens[0]

        # The tokens don't contain any whitespace, so ["FOO", "0"] could be
        # #define FOO0
        # #define FOO 0
        #
        # To try and mitigate this, we'll use the following rules:
        # 1. If the 2nd token is "(", assume function macro:  #define FOO(a) a*a
        # 2. If the 2nd token ISN'T "(", assume object macro: #define FOO BAR
        # 3. Add a space after every subsequent token
        #
        # Note: Use the debug output to confirm the converted macro
        if tokens_count > 1:
            if tokens[1] != "(":
                macro_text += " "
            macro_text += " ".join(tokens[1:])

        debug.line("parse_macro_definition", f"MACRO text=[{macro_text}]")
        macro_lines = code_lines.CodeLines()
        macro_lines.add_line(macro_text)

        return macro_lines

    def parse_macro_instantiation(self, node):
        debug.line("parse_macro_instantiation", f"MACRO spelling=[{node.spelling}] kind=[{node.kind}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")
        token_text = "".join([token.spelling for token in node.get_tokens()]) + ";"
        debug.line("parse_macro_instantiation", f"MACRO token text=[{token_text}]")
        macro_lines = code_lines.CodeLines()
        macro_lines.add_line(token_text)

        return macro_lines

    # =================================== Macros Convert functions [END]   ===================================

    # =================================== parse_STMT_funcs [BEGIN] ===================================

    # Just iteratively call parse_ast_node
    def parse_COMPOUND_STMT(self, node):

        debug.line("parse_COMPOUND_STMT", f">>> spelling=[{node.spelling}] kind=[{node.kind}]")

        stmt_lines = compound_statement.CompoundStatement()

        for child in node.get_children():
            stmt_lines.add_code_object(self.parse_ast_node(child))

        return stmt_lines

    # DECL_STMT is an "Adaptor class for mixing declarations with statements and expressions."
    # For now we'll just call parse_ast_node recursively with the children...
    def parse_DECL_STMT(self, node):
        stmt_lines = code_lines.CodeLines()

        for child in node.get_children():
            debug.line("parse_DECL_STMT", f"child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")
            stmt_lines.add_lines(self.parse_ast_node(child))

        return stmt_lines

    def parse_RETURN_STMT(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for return statement"
        return_value = children[0]

        tokens = [token.spelling for token in node.get_tokens()]
        return_value_tokens = [token.spelling for token in return_value.get_tokens()]

        if len(return_value_tokens) == 0:
            # Probably a macro - return as string
            debug.line("parse_RETURN_STMT", f"The return value has no tokens: using the top-level tokens=[{tokens[1:]}]")
            return_cvalue = code_lines.CodeLines(f"{' '.join(t for t in tokens[1:])}")
        else:
            return_cvalue = self.parse_ast_node(return_value)

        return_lines = return_cvalue.as_lines()
        return_lines[0] = "return " + return_lines[0]
        return_lines[-1] += ";"

        return code_lines.CodeLines(return_lines)

    def parse_IF_STMT(self, node):
        debug.line("parse_IF_STMT", f"IF spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

        children = list(node.get_children())
        child_count = len(children)
        assert child_count >= 2, f"Expected at least two children for if statement"

        if_expression = self.parse_ast_node(children[0])
        if_action = self.parse_ast_node(children[1])
        if_stmt = if_statement.IfStatement(if_expression, if_action)

        if child_count == 3:
            else_statement = self.parse_ast_node(children[2])
            if_stmt.add_else(else_statement)

        return if_stmt

    def parse_FOR_STMT(self, node):
        debug.line("parse_FOR_STMT", f"IF spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

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
            init_statement = self.parse_ast_node(child)

        # condition
        if cond_end_token_index-init_end_token_index > 1:
            child = next(children, None)
            assert child, f"condition node=[None]: init_end_token_index=[{init_end_token_index}] cond_end_token_index=[{cond_end_token_index}]"
            condition = self.parse_ast_node(child)

        # iteration_expression
        if close_paren_token_index - cond_end_token_index > 1:
            child = next(children, None)
            assert child, f"iteration_expression node=[None]: cond_end_token_index=[{cond_end_token_index}] close_paren_token_index=[{close_paren_token_index}]"
            iteration_expression = self.parse_ast_node(child)

        # statement
        child = next(children, None)
        assert child, f"For loop has no statement! node=[None]"
        statement = self.parse_ast_node(child)

        for_stmt = for_statement.ForStatement(init_statement, condition, iteration_expression, statement)

        return for_stmt

    parse_STMT_funcs = {
        clang.cindex.CursorKind.COMPOUND_STMT:  parse_COMPOUND_STMT,
        clang.cindex.CursorKind.DECL_STMT:      parse_DECL_STMT,
        clang.cindex.CursorKind.CASE_STMT:      parse_node_not_implemented,
        clang.cindex.CursorKind.DEFAULT_STMT:   parse_node_not_implemented,
        clang.cindex.CursorKind.IF_STMT:        parse_IF_STMT,
        clang.cindex.CursorKind.SWITCH_STMT:    parse_node_not_implemented,
        clang.cindex.CursorKind.WHILE_STMT:     parse_node_not_implemented,
        clang.cindex.CursorKind.DO_STMT:        parse_node_not_implemented,
        clang.cindex.CursorKind.FOR_STMT:       parse_FOR_STMT,
        clang.cindex.CursorKind.GOTO_STMT:      parse_node_not_implemented,
        clang.cindex.CursorKind.CONTINUE_STMT:  parse_node_not_implemented,
        clang.cindex.CursorKind.BREAK_STMT:     parse_node_not_implemented,
        clang.cindex.CursorKind.RETURN_STMT:    parse_RETURN_STMT,
        clang.cindex.CursorKind.NULL_STMT:      parse_node_not_implemented,
    }
    
    def parse_STMT_node(self, node):
        debug.line("parse_STMT_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.parse_STMT_funcs.keys():
            return self.parse_STMT_funcs[node.kind](self, node)
        else:
            return self.parse_node_ignored_kind(node)

    # =================================== parse_STMT_funcs [END]   ===================================

    # =================================== parse_DECL_funcs [BEGIN] ===================================

    # Assume this is just a declaration - the function body will be converted via a function class...
    def parse_FUNCTION_DECL(self, node):
        cfuncsig = cnode_utils.create_cfuncsig(node)
        cfuncsig.is_declaration = True
        return cfuncsig

    def parse_STRUCT_DECL(self, node):
        cstruct_arg = cnode_utils.create_cstruct_arg(node)
        return cstruct_arg

    def parse_PARM_DECL(self, node):
        return None

    def parse_TYPEDEF_DECL(self, node):
        typedef_type = node.underlying_typedef_type

        if typedef_type.kind == clang.cindex.TypeKind.POINTER:
            pointee = typedef_type.get_pointee()

            if pointee.kind == clang.cindex.TypeKind.FUNCTIONPROTO:
                # It's a function pointer
                cfuncsig_pointer = cnode_utils.create_cfuncsig_pointer(node)
                debug.line("parse_TYPEDEF_DECL", f"cfuncsig_pointer   = [{cfuncsig_pointer.as_string()}]")
                return cfuncsig_pointer
            
        elif typedef_type.kind == clang.cindex.TypeKind.ELABORATED:
            # We don't need to process e.g. "typedef struct proj_mapping proj_mapping;" as C++ doesn't need it!
            pass

        debug.line("parse_TYPEDEF_DECL", f"Ignoring node spelling=[{node.spelling}] typedef_type.kind=[{typedef_type.kind}]")
        return None

    def parse_FIELD_DECL(self, node):
        carg = cnode_utils.create_carg(node)
        arg_conv = arg_converter.ArgConverter(carg)
        cpparg = arg_conv.to_cpp_arg(self._conversion_data)
        
        debug.line("parse_FIELD_DECL", f"Converted spelling=[{node.spelling}] to cpparg=[{cpparg.as_string()}]")
        return cpparg

    def parse_VAR_DECL(self, node):
        cvariable = cnode_utils.create_carg(node)
        cvalue = None
        for child in node.get_children():
            if child.kind.is_expression():
                cvalue = self.parse_ast_node(child)                
            else:
                debug.line("parse_VAR_DECL", f"Ignoring child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")
        
        return variable_declaration.VariableDeclaration(cvariable, cvalue)

    parse_DECL_funcs = {
        clang.cindex.CursorKind.FUNCTION_DECL:      parse_FUNCTION_DECL,
        clang.cindex.CursorKind.STRUCT_DECL:        parse_STRUCT_DECL,
        clang.cindex.CursorKind.UNION_DECL:         parse_node_not_implemented,
        clang.cindex.CursorKind.FIELD_DECL:         parse_FIELD_DECL,
        clang.cindex.CursorKind.VAR_DECL:           parse_VAR_DECL,
        clang.cindex.CursorKind.PARM_DECL:          parse_PARM_DECL,
        clang.cindex.CursorKind.TYPEDEF_DECL:       parse_TYPEDEF_DECL,
        clang.cindex.CursorKind.TYPE_ALIAS_DECL:    parse_node_not_implemented,
    }

    def parse_DECL_node(self, node):
        debug.line("parse_DECL_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.parse_DECL_funcs.keys():
            return self.parse_DECL_funcs[node.kind](self, node)
        else:
            return self.parse_node_ignored_kind(node)

    # =================================== parse_DECL_funcs [END]   ===================================

    # =================================== parse_EXPR_funcs [BEGIN] ===================================

    # We just pass the request straight to the child...
    def parse_UNEXPOSED_EXPR(self, node):
        # There should be a single child node...
        child = list(node.get_children())[0]

        debug.line("parse_UNEXPOSED_EXPR", f"Passing to child: spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return self.parse_ast_node(child)

    # This is a recursive convertion...
    def parse_INIT_LIST_EXPR(self, node):
        #return cnode_utils.create_cinit_list(node)
        debug.line("parse_INIT_LIST_EXPR", f"spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")
        init_list_decl_spec = declaration_specifier.DeclSpec.from_decl_specifier_seq(node.type.spelling)
        cinit_list = init_list.InitList(init_list_decl_spec)

        for child in node.get_children():
            cvalue = self.parse_ast_node(child)
            if cvalue:
                cinit_list.add_entry(cvalue)

        return cinit_list

    def parse_INTEGER_LITERAL(self, node):
        if node.spelling:
            debug.line("parse_INTEGER_LITERAL", f"Got value from spelling=[{node.spelling}]")
            return code_lines.CodeLines(node.spelling)
        
        # We'll have to extract the value from the tokens
        tokens=[token.spelling for token in node.get_tokens()]
        if tokens:
            debug.line("parse_INTEGER_LITERAL", f"Got value from tokens=[{tokens[0]}]")
            return code_lines.CodeLines(tokens[0])
        
        assert False, f"Could not extract integer literal"

    def parse_STRING_LITERAL(self, node):
        return code_lines.CodeLines(node.spelling)

    def parse_PAREN_EXPR(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for paren expression"
        expression = children[0]

        expression_value = self.parse_ast_node(expression)
        c_paren_expr = paren_expression.ParenExpression(expression_value)
        return c_paren_expr

    def parse_CXX_UNARY_EXPR(self, node):
        keyword = node.spelling
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for unary expression"
        expression = children[0]

        expression_value = self.parse_ast_node(expression)
        c_unary_expr = unary_expression.UnaryExpression(keyword, expression_value)
        return c_unary_expr

    def parse_UNARY_OPERATOR(self, node):
        children = list(node.get_children())
        assert len(children) == 1, f"Expected exactly one child for unary operator"
        right_operand = children[0]

        # Tokenize and find the operator
        tokens = [token.spelling for token in node.get_tokens()]
        right_tokens = [token.spelling for token in right_operand.get_tokens()]

        # Find the operator by excluding operand tokens
        operator_tokens = [t for t in tokens if t not in right_tokens]
        debug.line("parse_UNARY_OPERATOR", f"Unary Operator = [{operator_tokens}]")
        #debug.line("parse_UNARY_OPERATOR", f"right_operand...")
        #cnode_utils.dump_node(right_operand)

        operator_str = "".join(t for t in operator_tokens)
        right_operand_cvalue = self.parse_ast_node(right_operand)

        c_unary_op = unary_operation.UnaryOperation(operator_str, right_operand_cvalue)
        return c_unary_op

    def parse_BINARY_OPERATOR(self, node):
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
            debug.line("parse_BINARY_OPERATOR", f"operator_tokens is empty!")
            debug.line("parse_BINARY_OPERATOR", f" -> tokens       = [{tokens}]")
            debug.line("parse_BINARY_OPERATOR", f" -> left_tokens  = [{left_tokens}]")
            debug.line("parse_BINARY_OPERATOR", f" -> right_tokens = [{right_tokens}]")
            assert False, "Binary operator: operator_tokens array should not be empty!"

        #operator_tokens = [t for t in tokens if t not in left_tokens and t not in right_tokens]
        operator_tokens = tokens[left_tokens_count:]

        operator_token = operator_tokens[0]
        debug.line("parse_BINARY_OPERATOR", f"operator_token=[{operator_token}] => operator_tokens=[{operator_tokens}]")

        left_operand_cvalue = self.parse_ast_node(left_operand)

        if len(operator_tokens) > 1:
            # The top level tokens don't match the right_operand tokens. This can happen if the top-level
            # contains (for example) a macro definition. 
            # For now we'll just take the top-level tokens as a string
            debug.line("parse_BINARY_OPERATOR", f"The top level tokens don't match the right_operand tokens: using the top-level tokens=[{operator_tokens[1:]}]")
            right_operand_cvalue = code_lines.CodeLines(f"{' '.join(t for t in operator_tokens[1:])}")
        else:
            right_operand_cvalue = self.parse_ast_node(right_operand)

        c_binary_op = binary_operation.BinaryOperation(left_operand_cvalue, operator_token, right_operand_cvalue)
        return c_binary_op

    def parse_COMPOUND_ASSIGNMENT_OPERATOR(self, node):
        debug.line("parse_COMPOUND_ASSIGNMENT_OPERATOR", f"Forwarding to parse_BINARY_OPERATOR spelling=[{node.spelling}] type=[{node.type.spelling}]")
        return self.parse_BINARY_OPERATOR(node)

    def parse_DECL_REF_EXPR(self, node):
        return code_lines.CodeLines(node.spelling)

    # This is simplified for now, need to come back to...
    def parse_MEMBER_REF_EXPR(self, node):
        member_name = node.spelling
        cstruct_member_access = struct_member_access.StructMemberAccess(None, member_name, None)

        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                parent_name = child.spelling
                cparent_struct_member_access = struct_member_access.StructMemberAccess(None, parent_name, None)
                cstruct_member_access.access = "->"
                cparent_struct_member_access.member = cstruct_member_access
                return cparent_struct_member_access
            
            debug.line("parse_MEMBER_REF_EXPR", f"*** IGNORING *** child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return None

    def parse_CALL_EXPR(self, node):
        tokens = [token.spelling for token in node.get_tokens()]
        debug.line("parse_CALL_EXPR", f"spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")
        debug.line("parse_CALL_EXPR", f"CALL_EXPR tokens=[{tokens}]")

        if "(" in tokens:
            debug.line("parse_CALL_EXPR", f"Found regular function call")
            cfunc_call = function_call.FunctionCall(node.spelling)

            for arg_node in node.get_arguments():
                arg_entry = self.parse_ast_node(arg_node)
                debug.line("parse_CALL_EXPR", f"arg_node spelling=[{arg_node.spelling}] type=[{arg_node.type.spelling}] kind=[{arg_node.kind}]")
                if arg_entry:
                    debug.line("parse_CALL_EXPR", f"arg_node arg_entry=[{arg_entry.as_string()}]")
                    cfunc_call.add_arg(arg_entry.as_string())
                else:
                    debug.line("parse_CALL_EXPR", f"arg_node arg_entry=[{arg_entry}] - IS THIS AN ERROR?")

            return cfunc_call
        
        # No "(" in the tokens, so this is not really a function call!
        # It is probably a copy constructor, so we'll parse and return the (first) child node as whatever type it is...
        debug.line("parse_CALL_EXPR", f"NOT a regular function call - parsing first child instead...")
        children = node.get_children()
        child = next(children, None)
        assert child, f"Expected a child node while parsing a non-regular function call!"

        return self.parse_ast_node(child)


    def parse_CSTYLE_CAST_EXPR(self, node):
        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                # We ignore the cast, and just return the object
                return code_lines.CodeLines(child.spelling)
            
            debug.line("parse_CSTYLE_CAST_EXPR", f"*** IGNORING *** child spelling=[{child.spelling}] type=[{child.type.spelling}] kind=[{child.kind}]")

        return None

    def parse_ARRAY_SUBSCRIPT_EXPR(self, node):
        debug.line("parse_ARRAY_SUBSCRIPT_EXPR", f"IF spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

        # We expect two children: the variable name and the index
        children = list(node.get_children())
        child_count = len(children)
        assert child_count == 2, f"Expected exactly two children for array subscription"

        name = self.parse_ast_node(children[0])
        index = self.parse_ast_node(children[1])

        arr_access = array_access.ArrayAccess(name, index)

        return arr_access

    def parse_CONDITIONAL_OPERATOR(self, node):
        debug.line("parse_CONDITIONAL_OPERATOR", f"spelling=[{node.spelling}] type=[{node.type.spelling}] kind=[{node.kind}]")

        # We expect three children: the condition, true branch and false branch
        children = list(node.get_children())
        child_count = len(children)
        assert child_count == 3, f"Expected exactly three children for conditional operator"

        bool_expression = self.parse_ast_node(children[0])
        true_expression = self.parse_ast_node(children[1])
        false_expression = self.parse_ast_node(children[2])

        cond_operation = conditional_operation.ConditionalOperation(bool_expression, true_expression, false_expression)

        return cond_operation

    parse_EXPR_funcs = {
        clang.cindex.CursorKind.UNEXPOSED_EXPR:                 parse_UNEXPOSED_EXPR,
        clang.cindex.CursorKind.DECL_REF_EXPR:                  parse_DECL_REF_EXPR,
        clang.cindex.CursorKind.MEMBER_REF_EXPR:                parse_MEMBER_REF_EXPR,
        clang.cindex.CursorKind.CALL_EXPR:                      parse_CALL_EXPR,
        clang.cindex.CursorKind.ARRAY_SUBSCRIPT_EXPR:           parse_ARRAY_SUBSCRIPT_EXPR,
        clang.cindex.CursorKind.CSTYLE_CAST_EXPR:               parse_CSTYLE_CAST_EXPR,
        clang.cindex.CursorKind.COMPOUND_LITERAL_EXPR:          parse_node_not_implemented,
        clang.cindex.CursorKind.INIT_LIST_EXPR:                 parse_INIT_LIST_EXPR,
        clang.cindex.CursorKind.INTEGER_LITERAL:                parse_INTEGER_LITERAL,
        clang.cindex.CursorKind.FLOATING_LITERAL:               parse_node_not_implemented,
        clang.cindex.CursorKind.STRING_LITERAL:                 parse_STRING_LITERAL,
        clang.cindex.CursorKind.CHARACTER_LITERAL:              parse_node_not_implemented,
        clang.cindex.CursorKind.PAREN_EXPR:                     parse_PAREN_EXPR,
        clang.cindex.CursorKind.CXX_UNARY_EXPR:                 parse_CXX_UNARY_EXPR,
        clang.cindex.CursorKind.UNARY_OPERATOR:                 parse_UNARY_OPERATOR,
        clang.cindex.CursorKind.BINARY_OPERATOR:                parse_BINARY_OPERATOR,
        clang.cindex.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR:   parse_COMPOUND_ASSIGNMENT_OPERATOR,
        clang.cindex.CursorKind.CONDITIONAL_OPERATOR:           parse_CONDITIONAL_OPERATOR,
    }

    def parse_EXPR_node(self, node):
        debug.line("parse_EXPR_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.parse_EXPR_funcs.keys():
            return self.parse_EXPR_funcs[node.kind](self, node)
        else:
            return self.parse_node_ignored_kind(node)

    # =================================== parse_EXPR_funcs [END]   ===================================

    # =================================== parse_REF_funcs [BEGIN]  ===================================

    def parse_TYPE_REF(self, node):
        return None

    parse_REF_funcs = {
        clang.cindex.CursorKind.TYPE_REF:       parse_TYPE_REF,
        clang.cindex.CursorKind.TEMPLATE_REF:   parse_node_not_implemented,
        clang.cindex.CursorKind.MEMBER_REF:     parse_node_not_implemented,
    }

    def parse_REF_node(self, node):
        debug.line("parse_REF_node", f"[{node.kind}] spelling=[{node.spelling}]")

        if node.kind in self.parse_REF_funcs.keys():
            return self.parse_REF_funcs[node.kind](self, node)
        else:
            return self.parse_node_ignored_kind(node)

    # =================================== parse_REF_funcs [END]    ===================================

