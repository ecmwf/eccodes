
import clang.cindex
import default.ccode as ccode
import cnode_utils
import os

class CParser:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._ccode = None

    parse_args = [
            #"-fparse-all-comments",
        ]
    
    # Override to create the right object
    def create_ccode(self):
        self._ccode = ccode.CCode(self._cfilename)

    # Default behaviour is to just add to the ccode object
    def parse_global_declaration(self, node):
        self._ccode.add_global_declaration(node)

    def parse_function_definition(self, node):
        cfuncsig = cnode_utils.create_cfuncsig(node)
        body = None

        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                body = child
                break

        assert body, f"Could not find body for function=[{node.spelling}]"

        self._ccode.add_function(cfuncsig, body)

    def parse_node(self, node):
        if node.kind.is_declaration:
            if node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.is_definition():
                self.parse_function_definition(node)
            else:
                self.parse_global_declaration(node)
        else:
            assert False, f"Unexpected node kind=[{node.kind}] spelling=[{node.spelling}] line=[{node.location.line}] col=[{node.location.column}]"

    def parse_root(self):
        self._root = self._translation_unit.cursor

        '''for inc in self._translation_unit.get_includes():
            if inc.depth == 1:
                debug.line("parse", f"Include=[{inc.include}] line=[{inc.location.line}]")'''

        for child in self._root.get_children():
            self.parse_node(child)

    # Perform shallow parse of AST and store in CCode object
    def parse(self):
        index = clang.cindex.Index.create()

        filepath = os.path.dirname(self._cfilename)
        if filepath == "":
            filepath = os.getcwd() + "/"
        
        self._translation_unit = index.parse(filepath + self._cfilename, args=CParser.parse_args)

        self.create_ccode()

        self.parse_root()

        return self._ccode
