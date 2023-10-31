import debug
import arg
import re

# Represent a function signature
class FuncSig:
    def __init__(self, return_type, name, args, template=None) -> None:
        self._return_type = return_type
        self._name = name
        self._args = args
        self._template = template
        self._static = False

    # Try and create a func sig from a string: will return None is unsuccessful...
    # Note: the sig_string can be multi-line...
    @classmethod
    def from_string(cls, sig_string):
        # Ignore typedefs - we don't want function pointers!
        if re.match(r"\s*\btypedef", sig_string, re.DOTALL):
            return None

        sig = None
        #m = re.match(r"(static\s*)?\b([^(]+)\s+(\w+)\s*\(([^(]+)\)", sig_string, re.DOTALL)
        m = re.match(r"(static\s*)?\b([^(]+)\s+(\w+)\s*\(([^(]*)\)", sig_string, re.DOTALL)
        if m:
            return_type = m.group(2)
            name = m.group(3)
            args = []
            for entry in [a.strip() for a in m.group(4).split(",")]:
                if not entry:
                    continue
                args.append(arg.Arg.from_func_arg_string(entry))
            
            sig = cls(return_type, name, args)
            if sig and m.group(1):
                sig.static = True

        return sig

    @property
    def name(self):
        return self._name

    @property
    def args(self):
        return self._args

    @property
    def return_type(self):
        return self._return_type

    @property
    def static(self):
        return self._static

    @static.setter
    def static(self, value):
        self._static = value

    @property
    def template(self):
        return self._template
    
    @template.setter
    def template(self, value):
        self._template = value

    def as_string(self):
        return f"{'static ' if self.static else ''}{self.return_type} {self.name}({', '.join([a.type + ' ' + a.name if a.name else '' for a in self.args if a])})"