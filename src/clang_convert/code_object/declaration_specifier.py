
import code_object.code_interface as code_interface
import utils.debug as debug
import re
from code_object.code_interface import NONE_VALUE

# Represents the full declaration specifier sequence (see C++ standard section 9.2 Specifiers) for an arg
# 
# This provides a more throrough way of defining an arg than type alone, and allows more precise mappings
#
# Create using the separate components, or use from_decl_specifier_seq to pass a string representing the
# full sequence, e.g. "static const unsigned int ** const"
class DeclSpec(code_interface.CodeInterface):
    def __init__(self, storage_class="", const_qualifier="", *, type, pointer) -> None:
        self._storage_class = storage_class
        self._const_qualifier = const_qualifier
        self._type = type
        self._pointer = pointer

    # Create a DeclSpec from a string representing the sequence, e.g. "const char*"
    # Pass None (or empty string) to return NONE_VALUE
    @classmethod
    def from_decl_specifier_seq(cls, decl_specifier_seq):
        if decl_specifier_seq == NONE_VALUE:
            return NONE_VALUE
        
        assert isinstance(decl_specifier_seq, str), f"Expected str, got [{decl_specifier_seq}]"

        decl_specifier_seq = decl_specifier_seq.strip()
        storage_class = ""
        const_qualifier = ""
        type = ""
        pointer = ""
        array_initial_capacity = "0"

        match_start = 0
        m = re.match(r"static\s+", decl_specifier_seq[match_start:])
        if m:
            storage_class = "static"
            match_start += m.end()
        
        # West const test...
        m = re.match(r"const\s+", decl_specifier_seq[match_start:])
        if m:
            const_qualifier = "const"
            match_start += m.end()

        m = re.match(r"[^\*&]+", decl_specifier_seq[match_start:])
        if m:
            type = m.group(0).strip()

            # If this is an array type, store the "[]" in the pointer variable
            if type.endswith("]"):
                array_match = re.search(r"\[[^\]]+\]", type)
                if array_match:
                    type = re.sub(re.escape(array_match.group(0)), "", type)
                    pointer = array_match.group(0)

            # East const test...
            if type.endswith(" const"):
                assert not const_qualifier
                const_qualifier = "const"
                type = type[:-6]

            match_start += m.end()

        m = re.match(r"[\*&]+.*", decl_specifier_seq[match_start:])
        if m:
            assert pointer=="", f"Pointer is not empty, value=[{pointer}]"
            pointer = m.group(0).replace(" ", "")
            match_start += m.end()

        assert match_start == len(decl_specifier_seq), f"Error parsing decl_specifier_seq [{decl_specifier_seq}]"

        #debug.line("from_decl_specifier_seq", f"decl_specifier_seq=[{decl_specifier_seq}]: storage_class=[{storage_class}] const_qualifier=[{const_qualifier}] type=[{type}] pointer=[{pointer}]")

        assert type

        return cls(storage_class=storage_class, const_qualifier = const_qualifier, type = type, pointer = pointer)

    # Create from an existing DeclSpec
    @classmethod
    def from_instance(cls, instance):
        return cls(storage_class=instance.storage_class, const_qualifier=instance.const_qualifier, type=instance.type, pointer=instance.pointer)

    @property
    def storage_class(self):
        return self._storage_class

    @property
    def const_qualifier(self):
        return self._const_qualifier

    @const_qualifier.setter
    def const_qualifier(self, value):
        self._const_qualifier = value

    @property
    def type(self):
        return self._type
    
    @type.setter
    def type(self, value):
        self._type = value

    @property
    def pointer(self):
        return self._pointer

    @pointer.setter
    def pointer(self, value):
        self._pointer = value

    def is_array_type(self):
        return self._pointer.startswith("[")

    def is_raw_pointer(self):
        return self._pointer.startswith("*")

    def array_size(self):
        if self.is_array_type() and len(self._pointer) > 2:
            return self._pointer[1:-1]

        return ""

    # ---------- Support for DeclSpec as a dict key: Begin ----------
    def __hash__(self):
        return hash((self._storage_class, self._const_qualifier, self.type, self._pointer))

    # Support for DeclSpec as a dict key
    def __eq__(self, other):
        if self is NONE_VALUE or other is NONE_VALUE:
            return self is other

        # Ensure the other object is an instance of FunctionArg for comparison.
        if not isinstance(other, DeclSpec):
            return False

        return self.storage_class == other.storage_class and self.const_qualifier == other.const_qualifier and self.pointer == other.pointer and self.type == other.type
    # ---------- Support for DeclSpec as a dict key: End   ----------

    # Return the full decl-specifier-seq as a (parsed) string (i.e. consistent spacing etc, not just what was passed in!)
    def as_lines(self):
        dss_string = ""
        if self._storage_class:
            dss_string += self._storage_class + " "

        if self._const_qualifier:
            dss_string += self.const_qualifier + " "

        dss_string += self._type

        if self._pointer:
            dss_string += self._pointer

        return [dss_string]
