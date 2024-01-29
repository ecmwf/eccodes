
# Represents the mapping from a buffer in C code {ptr, len} to a container class in C++
#
# For example char* v, size_t* len => std::string str

import code_object.arg as arg

class BufferMapping():
    def __init__(self, *, cbuffer, clength, cpp_container):
        assert isinstance(cbuffer, arg.Arg), f"cbuffer must be Arg instance, not [{type(cbuffer).__name__}]"
        assert isinstance(clength, arg.Arg), f"clength must be Arg instance, not [{type(clength).__name__}]"
        assert isinstance(cpp_container, arg.Arg), f"cpp_container must be Arg instance, not [{type(cpp_container).__name__}]"
        self._cbuffer = cbuffer
        self._clength = clength
        self._cpp_container = cpp_container

    @property
    def cbuffer(self):
        return self._cbuffer

    @property
    def clength(self):
        return self._clength

    @property
    def cpp_container(self):
        return self._cpp_container

    def as_string(self):
        return f"cbuffer=[{self._cbuffer.as_string()}] , clength=[{self._clength.as_string()}] -> cpp_container=[{self._cpp_container.as_string()}]"
    