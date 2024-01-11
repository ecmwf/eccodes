
import clang.cindex
import default.ccode as ccode
import cnode_utils
import os
import debug

class CParser:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._cfilepath = None
        self._ccode = None
        self._include_dirs = []

    parse_args = [
            #"-fparse-all-comments",
            #"-fdebug-macro",
            #"-fmacro-backtrace-limit=0",
            #"-E",
            #"-d1PP"
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
        if node.kind == clang.cindex.CursorKind.MACRO_DEFINITION:
            if node.location.file and not node.location.file.name.startswith("/usr/include") and not node.spelling.startswith("_"):
                self._ccode.add_macro_definition(node)
        elif node.kind == clang.cindex.CursorKind.MACRO_INSTANTIATION:
            if node.location.file and node.location.file.name == self._cfilepath + self._cfilename:
                self._ccode.add_macro_instantiation(node)
        elif node.kind == clang.cindex.CursorKind.INCLUSION_DIRECTIVE:
            pass
        elif node.kind.is_declaration:
            if node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.is_definition():
                self.parse_function_definition(node)

            # Add *ALL* nodes to the global declaration. 
            # This includes function definitions so we can determine whether to add a
            # forward declaration (nb all nodes are references so it isn't a big overhead)
            self.parse_global_declaration(node)

        else:
            assert False, f"Unexpected node kind=[{node.kind}] spelling=[{node.spelling}] line=[{node.location.line}] col=[{node.location.column}]"


    def parse_root(self):
        self._root = self._translation_unit.cursor

        '''for inc in self._translation_unit.get_includes():
            if inc.depth == 1:
                debug.line("parse", f"Include=[{inc.include}] line=[{inc.location.line}]")'''

        for inc in self._include_dirs:
            debug.line("parse_root", f"INCLUDE=[{inc}]")
        
        #self.extract_macros()

        for child in self._root.get_children():
            self.parse_node(child)

    # Perform shallow parse of AST and store in CCode object
    def parse(self):
        index = clang.cindex.Index.create()

        self._cfilepath = os.path.dirname(self._cfilename)
        if self._cfilepath == "":
            self._cfilepath = os.getcwd() + "/"
        
        self._include_dirs.append(self._cfilepath)

        debug.line("parse", f"self._cfilepath=[{self._cfilepath}] self._cfilename=[{self._cfilename}]")
        debug.line("parse", f"FILE=[{self._cfilepath + self._cfilename}]")

        parse_options = clang.cindex.TranslationUnit.PARSE_NONE
        # Enable this to get macros...
        parse_options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
        debug.line("parse", f"MACRO generation enabled [PARSE_DETAILED_PROCESSING_RECORD]")

        self._translation_unit = index.parse(self._cfilepath + self._cfilename, 
                                             args=CParser.parse_args,
                                             unsaved_files=None,
                                             options=parse_options)

        self.create_ccode()

        self.parse_root()

        # Debug - dump macros
        for node in self._ccode.macro_details.def_nodes:
            debug.line("parse", f"MACRO DEFN spelling=[{node.spelling}] loc=[{os.path.basename(node.location.file.name)}]")
        for node in self._ccode.macro_details.inst_nodes:
            debug.line("parse", f"MACRO INST spelling=[{node.spelling}] loc=[{os.path.basename(node.location.file.name)}]")

        return self._ccode
