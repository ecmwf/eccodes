
import funcsig
import funcsig_pointer
import arg
import clang.cindex

# Utilities for working with C AST Nodes

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
