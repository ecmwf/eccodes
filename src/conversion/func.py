import utils.debug as debug
import arg

class Function:
    def __init__(self, func_sig) -> None:
        self._func_sig = func_sig
        self._lines = []

    def is_empty(self):
        return len(self._lines) == 0

    def clear_lines(self):
        self._lines.clear()

    def add_line(self, line):
        # We split multi-lines before adding...
        for l in line.split("\n"):
            self._lines.append(l)

    @property
    def as_header_declaration(self):
        func_decl = ""
        if self._func_sig.template:
            func_decl += self._func_sig.template
        func_decl += f"{'static ' if self.static else ''}{self.return_type} {self.name}"
        func_decl += f"({', '.join([a.type + ' ' + a.name if a.name else '' for a in self.args if a])})"
        func_decl += self.const

        return func_decl

    # Allow different sig, e.g. adding Class:: prefix...
    @property   
    def as_source_declaration(self):
        func_decl = ""
        if self._func_sig.template:
            func_decl += self._func_sig.template
        func_decl += f"{'static ' if self.static else ''}{self.return_type} {self.name}"
        func_decl += f"({', '.join([a.type + ' ' + a.name if a.name else '' for a in self.args if a])})"
        func_decl += self.const

        return func_decl

    @property
    def func_sig(self):
        return self._func_sig

    @property
    def name(self):
        return self._func_sig.name

    @property
    def args(self):
        return self._func_sig.args

    # Return all args in a string
    @property
    def arg_string(self):
        return ", ".join([f"{a.type} {a.name}" for a in self.args if a])
    
    @property
    def return_type(self):
        return self._func_sig.return_type

    @property
    def static(self):
        return self._func_sig.static

    @property
    def const(self):
        return ""

    @property
    def template(self):
        return "" if self._func_sig._template is None else self._func_sig._template

    @property
    def code(self):
        if self._lines:
            assert self._lines[0].strip() == "{", "\n".join(self._lines)
            assert self._lines[-1].strip() == "}", "\n".join(self._lines)
        return self._lines[1:-1]

    @property
    def body(self):
        return "\n".join(self._lines)