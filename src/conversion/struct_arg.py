import utils.debug as debug
import re

# Represent the argument of a struct as defined in code, in order to manipulate/transform it
#
# It is recursive to allow structs within structs to be represented
#
# For example: *foo->bar->baz[4]
#
# At the top level: StructArg sa:
#   sa.access  = "*"
#   sa.name    = "foo"
#   sa.index   = ""
#
# Then, one level down, is sa.member:
#   sa.member.access  = "->"
#   sa.member.name    = "bar"
#   sa.member.index   = ""
#
# There is one more level in this example: sa.member.member:
#   sa.member.member.access  = "->"
#   sa.member.member.name    = "baz"
#   sa.member.member.index   = "[4]"
#
# Also:
#   from_string() will generate this object from an input string
#   as_string() will recursively create the string representation for the entire call chain...
#
class StructArg:
    def __init__(self, access, name, index, member=None) -> None:
        self._access = access
        self._name = name
        self._index = index
        self._member = member

    @property
    def access(self):
        return self._access

    @property
    def name(self):
        return self._name

    @property
    def index(self):
        return self._index
    
    @index.setter
    def index(self, value):
        assert value[0] == "[" and value[-1] == "]", f"Invalid index [{value}]"
        self._index = value

    @property
    def member(self):
        return self._member
    
    @member.setter
    def member(self, value):
        self._member = value

    # Generate a string to represent the assignment text
    def as_string(self):
        str = self._access if self._access is not None else ""
        str += self._name if self._name is not None else ""
        str += self._index if self._index is not None else ""
        if self.member:
            str += self.member.as_string()

        return str

# Find the first struct arg in the input string
#
# Returns: StructArg, match_start, match_end if a match is found, where:
#   StructArg   represents the matched struct argument
#   match_start is the index from the input string where the match starts
#   match_end   is the index from the input string where the match ends
#
# Returns: None, 0, 0 if no match
#
# Note: the first match group, (/\*), is to ensure we don't match inside a comment!
#
def cstruct_arg_from_string(input):

    cstruct_arg = None
    match_start = match_end = 0

    # match s.foo and s->foo
    access_match = "\.|->"

    # Ensure the name is a valid variable/function, i.e. doesn't begin with a number...
    name_match = "[a-zA-Z][\w\.]*"

    # Note: (?:\(\w+\))? is a non-capturing group that optionally matches (TEXT)
    #       and therefore allows us to capture function calls that result in 
    #       struct access, for example: grib_handle_of_accessor(a)->buffer->data;
    m = re.search(rf"(/\*)|([\*&])?({name_match}(?:\(\w+\))?)({access_match})(\w+)(\[[^\]]*\])?", input)

    if m and m.group(1) != "/*":
        access = m.group(2)
        name = m.group(3)
        index = None
        member = StructArg(access=m.group(4), name=m.group(5), index=m.group(6))

        match_start = m.start()
        match_end = m.end()

        cstruct_arg = StructArg(access, name, index, member)

        # Loop, adding any extra member sections (->foo[4]) that exist...
        next_member = cstruct_arg.member
        while m and m.end() < len(input):
            #m = re.match(rf"({access_match})({name_match})(\[[\w\d]*\])?", input[match_end:])
            m = re.match(rf"({access_match})({name_match})(\[[^\]]*\])?", input[match_end:])
            if not m: 
                break

            member = StructArg(access=m.group(1), name=m.group(2), index=m.group(3))
            match_end += m.end()

            next_member.member = member
            next_member = next_member.member

    if not cstruct_arg:
        debug.line("from_string", f"Input does not contain a struct member: {input}")

    return cstruct_arg, match_start, match_end

