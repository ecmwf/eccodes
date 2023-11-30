
import debug
import default.cppcode as cppcode
import clang.cindex
import cnode_utils
import funcsig_pointer

# Convert a C node (AST) into lines of C++ code
#
# Override this class to specialise the conversion process...

class CNodeConverter:
    def __init__(self, manifest_class) -> None:
        self._manifest_class = manifest_class

    # Perform the conversion and return a list of lines
    # CNode can be a single node or a list of nodes (for example, the global declaration is a list)
    # A list will be treated as a single block of code
    def convert(self, cnode, transforms):
        self._transforms = transforms

        if isinstance(cnode, list):
            return self.convert_node_list(cnode)
        else:
            return self.convert_node(cnode)

    def convert_node_list(self, nodes):
        lines = []

        for entry in nodes:
            converted_lines = self.convert_node(entry)
            if converted_lines:
                lines.append(converted_lines)

        return lines

    def convert_node(self, node):
        lines = []
        is_implemented = True

        match node.kind:
            case clang.cindex.CursorKind.STRUCT_DECL:
                self.convert_STRUCT_DECL(node)
            case clang.cindex.CursorKind.UNION_DECL:
                is_implemented = False
            case clang.cindex.CursorKind.FIELD_DECL:
                self.convert_FIELD_DECL(node)
            case clang.cindex.CursorKind.FUNCTION_DECL:
                lines = self.convert_FUNCTION_DECL(node)
            case clang.cindex.CursorKind.VAR_DECL:
                self.convert_VAR_DECL(node)
            case clang.cindex.CursorKind.PARM_DECL:
                self.convert_PARM_DECL(node)
            case clang.cindex.CursorKind.TYPEDEF_DECL:
                lines = self.convert_TYPEDEF_DECL(node)
            case clang.cindex.CursorKind.TYPE_ALIAS_DECL:
                is_implemented = False
            case clang.cindex.CursorKind.TYPE_REF:
                self.convert_TYPE_REF(node)
            case clang.cindex.CursorKind.TEMPLATE_REF:
                is_implemented = False
            case clang.cindex.CursorKind.MEMBER_REF:
                is_implemented = False
            case clang.cindex.CursorKind.DECL_REF_EXPR:
                self.convert_DECL_REF_EXPR(node)
            case clang.cindex.CursorKind.MEMBER_REF_EXPR:
                is_implemented = False
            case clang.cindex.CursorKind.CALL_EXPR:
                is_implemented = False
            case clang.cindex.CursorKind.INTEGER_LITERAL:
                is_implemented = False
            case clang.cindex.CursorKind.FLOATING_LITERAL:
                is_implemented = False
            case clang.cindex.CursorKind.STRING_LITERAL:
                self.convert_STRING_LITERAL(node)
            case clang.cindex.CursorKind.CHARACTER_LITERAL:
                is_implemented = False
            case clang.cindex.CursorKind.UNARY_OPERATOR:
                self.convert_UNARY_OPERATOR(node)
            case clang.cindex.CursorKind.ARRAY_SUBSCRIPT_EXPR:
                is_implemented = False
            case clang.cindex.CursorKind.BINARY_OPERATOR:
                is_implemented = False
            case clang.cindex.CursorKind.CONDITIONAL_OPERATOR:
                is_implemented = False
            case clang.cindex.CursorKind.CSTYLE_CAST_EXPR:
                is_implemented = False
            case clang.cindex.CursorKind.COMPOUND_LITERAL_EXPR:
                is_implemented = False
            case clang.cindex.CursorKind.INIT_LIST_EXPR:
                self.convert_INIT_LIST_EXPR(node)
            case clang.cindex.CursorKind.COMPOUND_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.CASE_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.DEFAULT_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.IF_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.SWITCH_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.WHILE_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.DO_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.FOR_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.GOTO_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.CONTINUE_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.BREAK_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.RETURN_STMT:
                is_implemented = False
            case clang.cindex.CursorKind.NULL_STMT:
                is_implemented = False
            case _:
                debug.line("convert_node", f"*** IGNORING *** spelling=[{node.spelling}] kind=[{node.kind}]")
                return []
        
        assert is_implemented, f"kind=[{node.kind}] not implemented"

        debug.line("convert_node", f"spelling=[{node.spelling}] kind=[{node.kind}]")
        return lines

    def convert_FUNCTION_DECL(self, node):
        cppfuncsig = self._transforms.cppfuncsig_for_cfuncname(node.spelling)
        if not cppfuncsig:
            cfuncsig = cnode_utils.create_cfuncsig(node)
            cfuncsig_conv = self._manifest_class.CFUNCSIG_CONVERTER(cfuncsig)
            mapping = cfuncsig_conv.create_funcsig_mapping(self._transforms)
            self._transforms.add_to_funcsig_mappings(mapping)
            cppfuncsig = mapping.cppfuncsig
            assert cppfuncsig, f"Error creating cppfuncsig from node [{node.spelling}]"

        return cppfuncsig.as_string() + ";"

    def convert_PARM_DECL(self, node):
        pass

    def convert_TYPE_REF(self, node):
        pass

    def convert_TYPEDEF_DECL(self, node):
        typedef_type = node.underlying_typedef_type

        if typedef_type.kind == clang.cindex.TypeKind.POINTER:
            pointee = typedef_type.get_pointee()

            if pointee.kind == clang.cindex.TypeKind.FUNCTIONPROTO:
                # It's a function pointer
                cfuncsig_pointer = cnode_utils.create_cfuncsig_pointer(node)
                cfuncsig_pointer_conv = self._manifest_class.CFUNCSIG_POINTER_CONVERTER(cfuncsig_pointer)
                mapping = cfuncsig_pointer_conv.create_funcsig_mapping(self._transforms)
                self._transforms.add_to_funcsig_pointer_mappings(mapping)
                cppfuncsig_pointer = mapping.cppfuncsig
                assert cppfuncsig_pointer, f"Error creating cppfuncsig_pointer from node [{node.spelling}]"
                
                debug.line("convert_TYPEDEF_DECL", f"cfuncsig_pointer   = [{cfuncsig_pointer.as_string()}]")
                debug.line("convert_TYPEDEF_DECL", f"cppfuncsig_pointer = [{cppfuncsig_pointer.as_string()}]")
                return cppfuncsig_pointer.as_string() + ";"
            
        debug.line("convert_TYPEDEF_DECL", f"Ignoring node spelling=[{node.spelling}] typedef_type.kind=[{typedef_type.kind}]")
        return ""

    def convert_STRUCT_DECL(self, node):
        pass

    def convert_FIELD_DECL(self, node):
        pass

    def convert_VAR_DECL(self, node):
        pass

    def convert_INIT_LIST_EXPR(self, node):
        pass

    def convert_STRING_LITERAL(self, node):
        pass

    def convert_UNARY_OPERATOR(self, node):
        pass

    def convert_DECL_REF_EXPR(self, node):
        pass
