
import default.default_cfile_parser as default_cfile_parser
import grib_accessor.grib_accessor_ccode as grib_accessor_ccode
import clang.cindex
import code_object.arg as arg

class GribAccessorCParser(default_cfile_parser.DefaultCFileParser):
    def __init__(self, cfilename) -> None:
        super().__init__(cfilename)
        self._accessor_class_name = self._cfilename.rstrip(".cc")
        self._accessor_name = self._accessor_class_name.replace("grib_accessor_class", "grib_accessor")

    base_class = "grib_accessor_class_gen"

    def create_ccode(self):
        self._ccode = grib_accessor_ccode.GribAccessorCCode(self._cfilename, self._accessor_name, self._accessor_class_name)

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

    # Overridden to parse the grib_accessor_* and _grib_accessor_class_* structs
    def parse_global_declaration(self, node):
        if node.spelling == self._accessor_name:
            if node.kind == clang.cindex.CursorKind.STRUCT_DECL:
                self.parse_grib_accessor_struct(node)
        elif node.spelling == "_" + self._accessor_class_name:
            self.parse_grib_accessor_class_struct(node)
        elif node.spelling == self._accessor_class_name:
            return # ignore!
        elif node.spelling == self.base_class:
            return # ignore!
        else:
            super().parse_global_declaration(node)
