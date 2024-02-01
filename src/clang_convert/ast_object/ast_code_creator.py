
import clang.cindex
import ast_object.ast_code as ast_code
import os
import utils.debug as debug

# Parse a single C file using Libclang and store the AST in an AstCode object

class AstCodeCreator:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._cfilepath = None
        self._include_dirs = []
        self._global_function_body = []

        self._parse_args = [
                #"-fparse-all-comments",
                #"-fdebug-macro",
                #"-fmacro-backtrace-limit=0",
                #"-E",
                #"-d1PP"
            ]

    def parse_node(self, node):
        if node.kind == clang.cindex.CursorKind.MACRO_DEFINITION:
            if node.location.file:
                if not node.location.file.name.startswith("/usr/include") and not node.spelling.startswith("_"):
                    self._ast_code.add_macro_definition(node)
                if node.location.file.name == self._cfilepath + self._cfilename:
                    # Add this definition to the global declaration
                    # Note: the preprocessor necessarily parses macros before everything else, so these will
                    #       ALWAYS appear at the top of the global declaration
                    self._ast_code.add_global_function_entry(node)
        elif node.kind == clang.cindex.CursorKind.MACRO_INSTANTIATION:
            if node.location.file and node.location.file.name == self._cfilepath + self._cfilename:
                self._ast_code.add_macro_instantiation(node)
        elif node.kind == clang.cindex.CursorKind.INCLUSION_DIRECTIVE:
            pass
        elif node.kind.is_declaration:
            if node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.is_definition():
                self._ast_code.add_function_node(node)

            # Parse *ALL* nodes to determine whether to add to the global declaration. 
            self._ast_code.add_global_function_entry(node)

        else:
            assert False, f"Unexpected node kind=[{node.kind}] spelling=[{node.spelling}] line=[{node.location.line}] col=[{node.location.column}]"

    def parse_root(self):
        self._root = self._translation_unit.cursor

        '''for inc in self._translation_unit.get_includes():
            if inc.depth == 1:
                debug.line("parse", f"Include=[{inc.include}] line=[{inc.location.line}]")'''

        for inc in self._include_dirs:
            debug.line("parse_root", f"INCLUDE=[{inc}]")
        
        for child in self._root.get_children():
            self.parse_node(child)

    # Perform shallow parse of AST and store in AstCode object
    def create(self):
        index = clang.cindex.Index.create()

        self._cfilepath = os.path.dirname(self._cfilename)
        if self._cfilepath == "":
            self._cfilepath = os.getcwd() + "/"
        
        self._include_dirs.append(self._cfilepath)

        debug.line("parse", f"self._cfilepath=[{self._cfilepath}] self._cfilename=[{self._cfilename}]")
        debug.line("parse", f"FILE=[{self._cfilepath + self._cfilename}]")

        #parse_options = clang.cindex.TranslationUnit.PARSE_NONE
        # Enable this to get macros...
        parse_options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
        debug.line("parse", f"MACRO generation enabled [PARSE_DETAILED_PROCESSING_RECORD]")

        # TEST - Set True to load file into memory (to allow macro manipulation etc)
        load_file_into_memory = False

        if load_file_into_memory:
            with open(self._cfilepath + self._cfilename, 'r') as file:
                file_contents = file.read()
            
            debug.line("FILE CONTENTS", file_contents)

            unsaved_file = ('in_memory_file.c', file_contents)
            self._translation_unit = index.parse('in_memory_file.c',
                                                args=self._parse_args,
                                                unsaved_files=[unsaved_file],
                                                options=parse_options)
        else:
            self._translation_unit = index.parse(self._cfilepath + self._cfilename, 
                                                args=self._parse_args,
                                                unsaved_files=None,
                                                options=parse_options)

        self._ast_code = ast_code.AstCode(self._cfilename)

        self.parse_root()

        # Debug - dump macros
        for node in self._ast_code.macro_details.def_nodes:
            debug.line("parse", f"MACRO DEFN spelling=[{node.spelling}] loc=[{os.path.basename(node.location.file.name)}]")
        for node in self._ast_code.macro_details.inst_nodes:
            debug.line("parse", f"MACRO INST spelling=[{node.spelling}] loc=[{os.path.basename(node.location.file.name)}] extent=[{node.extent.start.line}:{node.extent.start.column} -> {node.extent.end.line}:{node.extent.end.column}]")

        return self._ast_code
