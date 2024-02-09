
import utils.debug as debug
from enum import Enum, auto

import code_object.declaration_specifier as declaration_specifier
from code_object.code_interface import NONE_VALUE

# Functions and data to help the conversion data object

# Set the current state of the class:
# GLOBAL    - any new data is added to the global section (i.e. global function)
# LOCAL     - any new data is added to the local section, and can be erased (i.e. function-specific)
class ConversionDataState(Enum):
    GLOBAL = auto()
    LOCAL = auto()

# Define the match types for create_best_matching_cdecl_spec()
class DeclSpecMatchType(Enum):
    NONE                = 0b0000
    TYPE                = 0b0001
    POINTER             = 0b0010
    CONST               = 0b0100
    POINTER_AND_CONST   = 0b0110
    FULL                = 0b1111

# Find the closest match for the cdecl_spec in the supplied matches
# and create a new DeclSpec that best fits the match, which is returned
#
# The following rules are applied:
# 1. Type must match
# 2. POINTER and CONST values must match if C version exists
# 3. POINTER and CONST can have values if C equivalent is empty
#
# Returns a new DeclSpec instance and a match value, with the match value meaning:
#   FULL                : Complete match returned (not handled here)
#   POINTER_AND_CONST   : Matched type and BOTH pointer and const
#   POINTER             : Matched type and pointer
#   CONST               : Matched type and const
#   TYPE                : Matched type only
#   NONE                : No  match (returns None)
#
# NOTE: The entries in the matches list must already match the type
def create_best_matching_cdecl_spec(cdecl_spec, matches):
    key = 0
    value = 1
    matches_count = len(matches)

    debug.line("create_best_matching_cdecl_spec", f"cdecl_spec=[{debug.as_debug_string(cdecl_spec)}] cdecl_spec.type=[{debug.as_debug_string(cdecl_spec.type)}] cdecl_spec.pointer=[{debug.as_debug_string(cdecl_spec.pointer)}] ")
    debug.line("create_best_matching_cdecl_spec", f"matches_count=[{matches_count}]")

    for match in matches:
        debug.line("create_best_matching_cdecl_spec", f" Match key=[{debug.as_debug_string(match[key])}] value=[{debug.as_debug_string(match[value])}]")
    if matches_count == 0:
        return None, DeclSpecMatchType.NONE
    
    scored_matches = {}

    # Score all the matches
    for match in matches:
        score = DeclSpecMatchType.NONE.value

        if cdecl_spec.pointer == match[key].pointer:
            score |= DeclSpecMatchType.POINTER.value
            debug.line("create_best_matching_cdecl_spec", f" Pointer Match: entry key=[{debug.as_debug_string(match[key])}] score=[{score}]")
        elif cdecl_spec.is_array_type() and match[key].pointer == "*":
            score |= DeclSpecMatchType.POINTER.value
            debug.line("create_best_matching_cdecl_spec", f" Pointer Match: entry key=[{debug.as_debug_string(match[key])}] score=[{score}]")
        if cdecl_spec.const_qualifier == match[key].const_qualifier:
            score |= DeclSpecMatchType.CONST.value
            debug.line("create_best_matching_cdecl_spec", f" Const Match: entry key=[{debug.as_debug_string(match[key])}] score=[{score}]")
        scored_matches[match] = score

    for k,v in scored_matches.items():
        debug.line("create_best_matching_cdecl_spec", f" Scored Match: key=[{debug.as_debug_string(k[key])}] score=[{v}]")

    # Rank the results
    ranked_scored_matches = sorted(scored_matches.items(), key=lambda x: x[1], reverse=True)
    result, score = ranked_scored_matches[0]
    debug.line("create_best_matching_cdecl_spec", f" Result: key=[{debug.as_debug_string(result[key])}] value=[{debug.as_debug_string(result[value])}] match=[{score}]")

    # Create the new DeclSpec
    if result[value] == NONE_VALUE:
        new_decl_spec = NONE_VALUE
    else:
        new_decl_spec = declaration_specifier.DeclSpec.from_instance(result[value])
        if not (score & DeclSpecMatchType.POINTER.value):
            new_decl_spec.pointer = cdecl_spec.pointer

        if not (score & DeclSpecMatchType.CONST.value):
            new_decl_spec.const_qualifier = cdecl_spec.const_qualifier

    debug.line("create_best_matching_cdecl_spec", f" new_decl_spec=[{debug.as_debug_string(new_decl_spec)}] match=[{score}]")
    return new_decl_spec, DeclSpecMatchType(score)
