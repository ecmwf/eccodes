
import utils.debug as debug
import ast_object.ast_parser as ast_parser
import ast_object.ast_utils as ast_utils
import default.default_ccode as default_ccode

# Convert an AstCode object into a CCode object
#
class DefaultAstCodeConverter:
    def __init__(self, ast_code) -> None:
        self._ast_code = ast_code
        self._ast_parser = ast_parser.AstParser()

    # Override to create the required CCode object
    def create_ccode(self):
        cfilename = self._ast_code.cfilename
        self._ccode = default_ccode.DefaultCCode(cfilename)

    # Entry point...
    def convert(self):
        self.create_ccode()
        self.convert_global_function_nodes()
        self.convert_function_nodes()
        self.convert_macro_details()

        return self._ccode

    def convert_global_function_nodes(self):
        global_function_body = self._ast_parser.to_ccode_objects(self._ast_code.global_function_nodes)
        self._ccode.set_global_function_body(global_function_body)
        debug.line("convert_global_function_nodes", global_function_body.as_lines())

    def convert_function_nodes(self):
        for ast_func_node in self._ast_code.function_nodes:
            cfuncsig = ast_utils.create_cfuncsig(ast_func_node)
            body_node = ast_utils.find_function_body_node(ast_func_node)
            body = self._ast_parser.to_ccode_objects(body_node, self._ast_code.macro_details)
            self._ccode.add_function(cfuncsig, body)
            debug.line("convert_function_nodes", cfuncsig.as_lines() + body.as_lines())

    def convert_macro_details(self):
        # For now, just set the AST Node...
        self._ccode.macro_details = self._ast_code.macro_details
