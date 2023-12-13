
import debug
import code_object.cppfunction as cppfunction

# Stores information about Macro definition and use

class MacroDetails:
    def __init__(self) -> None:
        self._def_nodes = []
        self._inst_nodes = []

    @property
    def def_nodes(self):
        return self._def_nodes

    @property
    def inst_nodes(self):
        return self._inst_nodes

    def add_definition(self, def_node):
        self._def_nodes.append(def_node)

    def add_instantiation(self, inst_node):
        self._inst_nodes.append(inst_node)

    def is_instantiation(self, ast_node):
        for node in self._inst_nodes:
            if (node.extent.start.line   == ast_node.extent.start.line and 
                node.extent.start.column == ast_node.extent.start.column and
                node.extent.end.line     == ast_node.extent.end.line and 
                node.extent.end.column   == ast_node.extent.end.column):
                return True
            
        return False

    ''' 
    # In case it is needed...
    def process_macro_definition(self, node):
        param_tokens = []
        body_tokens = []
        is_func_macro=False

        token_iter = node.get_tokens()
        token_iter_end = "end"
        token = next(token_iter, token_iter_end)
        assert isinstance(token, clang.cindex.Token) and token.spelling == node.spelling, f"First token is not the macro name [{node.spelling}]"
        macro_name_token = token

        token = next(token_iter, token_iter_end)
        if token != token_iter_end and token.spelling == "(":
            # Must be a function macro
            token = next(token_iter, token_iter_end)
            while token != token_iter_end:
                if token.spelling == ")":
                    break
                param_tokens.append(token)
                token = next(token_iter, token_iter_end)

            assert token != token_iter_end, f"Function macro ill-formed - could not find \")\" at end of param list!"

            token = next(token_iter, token_iter_end)
            while token != token_iter_end:
                body_tokens.append(token)
                token = next(token_iter, token_iter_end)

            body_spellings = [token.spelling for token in body_tokens]
            if all(param.spelling in body_spellings for param in param_tokens):
                is_func_macro=True

        debug.line("process_macro_definition", f"      Macro Name=[{node.spelling}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")
        debug.line("process_macro_definition", f"      Name Token=[{macro_name_token.spelling}] extent=[{macro_name_token.extent.start.line}:{macro_name_token.extent.start.column} -> {macro_name_token.extent.end.line}:{macro_name_token.extent.end.column}]")
        for token in param_tokens:
            debug.line("process_macro_definition", f"      Param Token=[{token.spelling}] extent=[{token.extent.start.line}:{token.extent.start.column} -> {token.extent.end.line}:{token.extent.end.column}]")
        for token in body_tokens:
            debug.line("process_macro_definition", f"      Body Token=[{token.spelling}] extent=[{token.extent.start.line}:{token.extent.start.column} -> {token.extent.end.line}:{token.extent.end.column}]")
        debug.line("process_macro_definition", f"      is_func_macro=[{is_func_macro}]")
    '''
