# Provides mapping information from C to C++ function signatures

class FuncSigMapping():
    def __init__(self, cfuncsig, cppfuncsig, arg_indexes=None):
        self._cfuncsig = cfuncsig
        self._cppfuncsig = cppfuncsig
        self._arg_indexes = arg_indexes

    @property
    def cfuncsig(self):
        return self._cfuncsig

    @property
    def cppfuncsig(self):
        return self._cppfuncsig

    @property
    def arg_indexes(self):
        return self._arg_indexes
    