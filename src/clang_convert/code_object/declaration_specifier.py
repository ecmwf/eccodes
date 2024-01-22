
import code_object.code_interface as code_interface
import utils.debug as debug
import re

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

    @classmethod
    def from_decl_specifier_seq(cls, decl_specifier_seq):
        assert decl_specifier_seq, f"Unexpected decl_specifier_seq=[{decl_specifier_seq}]"
        decl_specifier_seq = decl_specifier_seq.strip()
        storage_class = ""
        const_qualifier = ""
        type = ""
        pointer = ""

        match_start = 0
        m = re.match(r"static\s+", decl_specifier_seq[match_start:])
        if m:
            storage_class = "static"
            match_start += m.end()
        
        m = re.match(r"const\s+", decl_specifier_seq[match_start:])
        if m:
            const_qualifier = "const"
            match_start += m.end()

        m = re.match(r"[^\*]+", decl_specifier_seq[match_start:])
        if m:
            type = m.group(0).strip()

            # If this is an array type, store the "[]" in the pointer variable
            if type.endswith("]"):
                type = re.sub(r"\[[^\]]+\]", "", type)
                pointer = "[]"

            match_start += m.end()

        m = re.match(r"\*+.*", decl_specifier_seq[match_start:])
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
        return self.pointer == "[]"

    # Support for DeclSpec as a dict key
    def __hash__(self):
        return hash((self._storage_class, self._const_qualifier, self.type, self._pointer))

    # Support for DeclSpec as a dict key
    def __eq__(self, other):
        if isinstance(other, DeclSpec):
            return self.storage_class == other.storage_class and self.const_qualifier == other.const_qualifier and self.pointer == other.pointer and self.type == other.type
        return False

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
