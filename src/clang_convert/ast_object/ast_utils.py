
import clang.cindex
import utils.debug as debug
import code_object.funcsig as funcsig
import code_object.funcsig_pointer as funcsig_pointer
import code_object.arg as arg
import code_object.init_list as init_list
import code_object.declaration_specifier as declaration_specifier
import code_object.struct_arg as struct_arg
import code_object.struct_member_access as struct_member_access
import code_object.literal as literal
import code_object.array_access as array_access

# Utilities for working with C AST Nodes

# tokens string can be:
# "flat" to show a flat summary
# "list" to show a detailed list
# "truncate" to show the first 10 tokens
# "" to not show tokens
def dump_node(cnode, depth=0, tokens="truncate"):
    truncate_depth = 10
    debug.line("dump_node", f"{' ' * depth}[{depth}:{cnode.kind}] spelling=[{cnode.spelling}] type=[{cnode.type.spelling}] extent=[{cnode.extent.start.line}:{cnode.extent.start.column}]->[{cnode.extent.end.line}:{cnode.extent.end.column}]")
    if tokens == "flat":
        debug.line("dump_node", f"{' ' * depth} -> tokens=[{[token.spelling for token in cnode.get_tokens()]}]")
    elif tokens == "list":
        for token in cnode.get_tokens():
            debug.line("dump_node", f"{' ' * depth} -> token=[{token.spelling}] extent=[{token.extent.start.line}:{token.extent.start.column} -> {token.extent.end.line}:{token.extent.end.column}]")
    elif tokens == "truncate":
        token_list = [token.spelling for token in cnode.get_tokens()]
        debug.line("dump_node", f"{' ' * depth} -> tokens[:{truncate_depth}]=[{token_list[:truncate_depth]}]")


    for child in cnode.get_children():
        dump_node(child, depth+1, tokens)

# Create a C FuncSig object from a FUNCTION_DECL node
def create_cfuncsig(cnode):
    if cnode.kind == clang.cindex.CursorKind.FUNCTION_TEMPLATE:
        return create_template_cfuncsig(cnode)

    assert cnode.kind == clang.cindex.CursorKind.FUNCTION_DECL, f"cnode kind=[{cnode.kind}], expected FUNCTION_DECL"

    cargs = []
    for entry in cnode.get_arguments():
        carg = arg.Arg(entry.type.spelling, entry.spelling, is_func_arg=True)
        cargs.append(carg)

    return funcsig.FuncSig(cnode.result_type.spelling, cnode.spelling, cargs)

# Create a C FuncSig object from a FUNCTION_TEMPLATE node
def create_template_cfuncsig(cnode):
    assert cnode.kind == clang.cindex.CursorKind.FUNCTION_TEMPLATE, f"cnode kind=[{cnode.kind}], expected FUNCTION_TEMPLATE"

    cargs = []
    ctemplate_type_params = []

    for child in cnode.get_children():
        if child.kind == clang.cindex.CursorKind.TEMPLATE_TYPE_PARAMETER:
            ctemplate_type_params.append(child.spelling)
        elif child.kind == clang.cindex.CursorKind.PARM_DECL:
            carg = arg.Arg(child.type.spelling, child.spelling, is_func_arg=True)
            cargs.append(carg)

    return funcsig.FuncSig(cnode.result_type.spelling, cnode.spelling, cargs, ctemplate_type_params)

# Extract the node representing the body of a function, or assert!
def find_function_body_node(cnode):
    for child in cnode.get_children():
        if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            return child

    assert False, f"Could not find body for function=[{cnode.spelling}] type=[{cnode.type.spelling}]"

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


symbol_tokens = ["[", "]", "->", "."]
# Iterate over list of tokens and create the appropriate code_objects
def create_code_object_from_tokens(tokens, depth=0):
    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}:IN] tokens={tokens}")

    assert len(tokens) != 0
    name = ""

    # Build the name from the non-symbol token(s)
    for tok in list(tokens):
        if tok not in symbol_tokens:
            name += tokens.pop(0)
        else:
            debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] name = [{debug.as_debug_string(name)}] tokens={tokens}")
            break
       
    assert name and name not in symbol_tokens, f"Name token can't be in {symbol_tokens}!"
    name_obj = literal.Literal(name)
    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] name_obj=[{debug.as_debug_string(name_obj)}] [{type(name_obj)}]")

    if len(tokens) == 0:
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}:OUT] No more tokens: name_obj=[{debug.as_debug_string(name_obj)}] [{type(name_obj)}]")
        return name_obj
    
    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}.1] tokens={tokens}")
    symbol = tokens.pop(0)

    if symbol == "[":
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] Found a [ - creating an array access object, tokens={tokens}")
        index_obj = create_code_object_from_tokens(tokens, depth+1)
        array_access_obj = array_access.ArrayAccess(name_obj, index_obj)
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] array_access_obj=[{debug.as_debug_string(array_access_obj)}] [{type(array_access_obj)}] tokens={tokens}")
        name_obj = array_access_obj
        if len(tokens) == 0:
            return name_obj
        symbol = tokens.pop(0)

    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}.2] tokens={tokens}")

    if symbol == "]":
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}:OUT] Found ], name_obj=[{debug.as_debug_string(name_obj)}] tokens={tokens}")
        return name_obj

    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}.3] tokens={tokens}")

    if symbol in ["->", "."]:
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] Creating StructMemberAccess from tokens={tokens}")
        member_name_obj = create_code_object_from_tokens(tokens, depth+1)
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] member_name_obj=[{debug.as_debug_string(member_name_obj)}]")
        member_obj = struct_member_access.StructMemberAccess(symbol, member_name_obj)
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}] [{symbol}] member_obj=[{debug.as_debug_string(member_obj)}]")
        struct_member_access_obj = struct_member_access.StructMemberAccess("", name_obj, member_obj)
        debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}:OUT] struct_member_access_obj=[{debug.as_debug_string(struct_member_access_obj)}] [{type(struct_member_access_obj)}]")
        return struct_member_access_obj

    debug.line("create_code_object_from_tokens", f"{' ' * depth}[{depth}.4] tokens={tokens}")
    assert False, "Didn't expect to get here! tokens={tokens}"