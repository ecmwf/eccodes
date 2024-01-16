
import clang.cindex
import utils.debug as debug
import code_object.funcsig as funcsig
import code_object.funcsig_pointer as funcsig_pointer
import code_object.arg as arg
import code_object.init_list as init_list
import code_object.declaration_specifier as declaration_specifier
import code_object.struct_arg as struct_arg

# Utilities for working with C AST Nodes

# tokens string can be:
# "flat" to show aflat summary
# "list" to show a detailed list
# "" to not show tokens
def dump_node(cnode, depth=0, tokens="flat"):
    debug.line("dump_node", f"{' ' * depth}[{cnode.kind}] spelling=[{cnode.spelling}] type=[{cnode.type.spelling}] extent=[{cnode.extent.start.line}:{cnode.extent.start.column}]->[{cnode.extent.end.line}:{cnode.extent.end.column}]")
    if tokens == "flat":
        debug.line("dump_node", f"{' ' * depth} -> tokens=[{[token.spelling for token in cnode.get_tokens()]}]")
    elif tokens == "list":
        for token in cnode.get_tokens():
            debug.line("dump_node", f"{' ' * depth} -> token=[{token.spelling}] extent=[{token.extent.start.line}:{token.extent.start.column} -> {token.extent.end.line}:{token.extent.end.column}]")

    for child in cnode.get_children():
        dump_node(child, depth+1)

# Create a C FuncSig object from a FUNCTION_DECL node
def create_cfuncsig(cnode):
    assert cnode.kind == clang.cindex.CursorKind.FUNCTION_DECL, f"cnode kind=[{cnode.kind}], expected FUNCTION_DECL"

    cargs = []
    for entry in cnode.get_arguments():
        carg = arg.Arg(entry.type.spelling, entry.spelling, is_func_arg=True)
        cargs.append(carg)

    return funcsig.FuncSig(cnode.result_type.spelling, cnode.spelling, cargs)

# Create a C FuncSigPointer object from a TYPEDEF_DECL node
def create_cfuncsig_pointer(cnode):
    assert cnode.kind == clang.cindex.CursorKind.TYPEDEF_DECL, f"cnode kind=[{cnode.kind}], expected TYPEDEF_DECL"

    pointee = cnode.underlying_typedef_type.get_pointee()
    assert pointee.kind == clang.cindex.TypeKind.FUNCTIONPROTO, f"pointee kind=[{pointee.kind}], expected FUNCTIONPROTO"

    retval = pointee.get_result().spelling
    name = cnode.spelling

    cargs = []
    for entry in cnode.get_children():
        carg = arg.Arg(entry.type.spelling, entry.spelling, is_func_arg=True)
        cargs.append(carg)

    return funcsig_pointer.FuncSigPointer(retval, name, cargs)

# Create a C Arg from a VAR_DECL or FIELD_DECL node
def create_carg(cnode):
    assert cnode.kind in [clang.cindex.CursorKind.VAR_DECL, clang.cindex.CursorKind.FIELD_DECL], f"Can't create C Arg from cnode kind=[{cnode.kind}]"

    return arg.Arg(cnode.type.spelling, cnode.spelling)

# Create a C Struct object from a STRUCT_DECL node
def create_cstruct_arg(cnode):
    assert cnode.kind == clang.cindex.CursorKind.STRUCT_DECL, f"cnode kind=[{cnode.kind}], expected STRUCT_DECL"

    cmembers = []
    for child in cnode.get_children():
        carg = create_carg(child)
        cmembers.append(carg)

    return struct_arg.StructArg(cnode.spelling, cmembers)

# Create C init list from INIT_LIST_EXPR node
def create_cinit_list(cnode):
    assert cnode.kind == clang.cindex.CursorKind.INIT_LIST_EXPR, f"cnode kind=[{cnode.kind}], expected INIT_LIST_EXPR"

    init_list_decl_spec = declaration_specifier.DeclSpec.from_decl_specifier_seq(cnode.type.spelling)
    cinit_list = init_list.InitList(init_list_decl_spec)

    for entry in cnode.get_children():
        debug.line("create_cinit_list", f"Entry: spelling=[{entry.spelling}] type=[{entry.type.spelling}] kind=[{entry.kind}]")
        entry_args = []
        if entry.kind == clang.cindex.CursorKind.INIT_LIST_EXPR:
            for entry_arg in entry.get_children():
                debug.line("create_cinit_list", f" -> entry_arg: spelling=[{entry_arg.spelling}] type=[{entry_arg.type.spelling}] kind=[{entry_arg.kind}]")
                entry_decl_spec = declaration_specifier.DeclSpec.from_decl_specifier_seq(entry_arg.type.spelling)
                if entry_arg.kind == clang.cindex.CursorKind.UNARY_OPERATOR:
                    for entry_arg_child in entry_arg.get_children():
                        if entry_arg_child.kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                            entry_name = entry_arg_child.spelling
                else:
                    entry_name = entry_arg.spelling
                entry_arg = arg.Arg(entry_decl_spec, entry_name)
                entry_args.append(entry_arg)
        cinit_list.add_entry(entry_args)

    return cinit_list

