
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
        return self.instantiation_node_for(ast_node) is not None

    def instantiation_node_for(self, ast_node):
        for node in self._inst_nodes:
            if (node.extent.start.line   == ast_node.extent.start.line and 
                node.extent.start.column == ast_node.extent.start.column and
                node.extent.end.line     == ast_node.extent.end.line and 
                node.extent.end.column   == ast_node.extent.end.column):
                return node
            
        return None

