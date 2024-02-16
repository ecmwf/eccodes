
import utils.debug as debug
import ast_object.ast_macro_details as ast_macro_details
import os
import ast_object.ast_utils as ast_utils

# Represents a coherent unit of code that needs to be parsed together: usually a single .cc file
#
# Everything is stored as AST nodes
class AstCode:
    def __init__(self, cfilename) -> None:
        self._cfilename = cfilename
        self._global_function_nodes = []
        self._function_nodes = []
        self._macro_details = ast_macro_details.AstMacroDetails()

    @property
    def cfilename(self):
        return self._cfilename

    @property
    def global_function_nodes(self):
        return self._global_function_nodes

    def add_global_function_entry(self, node):
        self._global_function_nodes.append(node)

    @property
    def function_nodes(self):
        return self._function_nodes

    def add_function_node(self, func_node):
        self._function_nodes.append(func_node)

    @property
    def macro_details(self):
        return self._macro_details

    def add_macro_definition(self, def_node):
        debug.line("add_macro_definition", f"Adding MACRO DEFN spelling=[{def_node.spelling}] loc=[{os.path.basename(def_node.location.file.name)}] extent={ast_utils.node_extent(def_node)}")
        self._macro_details.add_definition(def_node)

    def add_macro_instantiation(self, inst_node):
        debug.line("add_macro_instantiation", f"Adding MACRO INST spelling=[{inst_node.spelling}] loc=[{os.path.basename(inst_node.location.file.name)}] extent={ast_utils.node_extent(inst_node)}")
        self._macro_details.add_instantiation(inst_node)
