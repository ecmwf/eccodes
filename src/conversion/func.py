import debug
import arg

global_function_name = "Global"

class Function:
    def __init__(self, func_sig) -> None:
        self._func_sig = func_sig
        self._lines = []

    def is_empty(self):
        return len(self._lines) == 0

    def clear_lines(self):
        self._lines.clear()

    def add_line(self, line):
        self._lines.append(line)

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