
# Contains the index [position] for args representing e.g. {ptr, len} for C buffers

class ArgIndexes():
    def __init__(self, *, cbuffer=None, clength=None, cpp_container=None):
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

