
import default.default_ast_code_converter as default_ast_code_converter
import code_object.code_objects as code_objects
import grib_accessor.grib_accessor_ccode as grib_accessor_ccode
import clang.cindex
import code_object.arg as arg
import utils.debug as debug

class GribAccessorAstCodeConverter(default_ast_code_converter.DefaultAstCodeConverter):
    def __init__(self, ast_code) -> None:
        super().__init__(ast_code)

    base_class = "grib_accessor_class_gen"

    def create_ccode(self):
        cfilename = self._ast_code.cfilename
        self._accessor_class_name = cfilename.rstrip(".cc")
        self._accessor_name = self._accessor_class_name.replace("grib_accessor_class", "grib_accessor")
        self._ccode = grib_accessor_ccode.GribAccessorCCode(cfilename, self._accessor_name, self._accessor_class_name)

    # Overridden so we can parse each node individually and extract the class information...
    def convert_global_function_nodes(self):
        global_function_body = code_objects.CodeObjects()
        for node in self._ast_code.global_function_nodes:
            ccode = self.parse_global_function_node(node)
            if ccode:
                global_function_body.add_code_object(ccode)

        self._ccode.set_global_function_body(global_function_body)
        debug.line("convert_global_function_nodes", global_function_body.as_lines())

    # Return either the ccode to add, or None
    def parse_global_function_node(self, node):
        if node.spelling == self._accessor_name:
            if node.kind == clang.cindex.CursorKind.STRUCT_DECL:
                self.parse_grib_accessor_struct(node)
        elif node.spelling == "_" + self._accessor_class_name:
            self.parse_grib_accessor_class_struct(node)
        elif node.spelling == self._accessor_class_name:
            pass # ignore!
        elif node.spelling == self.base_class:
            pass # ignore!
        elif node.kind == clang.cindex.CursorKind.UNEXPOSED_DECL:
            pass # ignore - this is an #include !
        else:
            return self._ast_parser.to_ccode_objects(node, self._ast_code.macro_details)

        return None

    def parse_grib_accessor_struct(self, node):
        for child in node.get_children():
            if child.spelling == "att" or child.kind != clang.cindex.CursorKind.FIELD_DECL:
                continue
                #debug.line("parse_grib_accessor_struct", f"Ignoring member=[{node.spelling}] kind=[{child.kind}]")
            else:
                cmember = arg.Arg(child.type.spelling, child.spelling)
                self._ccode.add_data_member(cmember)

    # Parse the first entry in the initializer list for the super class name, and the second entry for the string name
    def parse_grib_accessor_class_struct(self, node):
        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.INIT_LIST_EXPR:
                init_list_iter = child.get_children()
                super_entry = next(init_list_iter)
                for super_entry_child in super_entry.get_children():
                    if super_entry_child.kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                        self._ccode._super_class_name = super_entry_child.spelling
                name_entry = next(init_list_iter)
                self._ccode._accessor_class_short_name = name_entry.spelling.replace("\"", "")
