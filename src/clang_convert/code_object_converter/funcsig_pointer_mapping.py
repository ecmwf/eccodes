# Provides mapping information from C to C++ function pointer signatures

class FuncSigPointerMapping():
    def __init__(self, cfuncsig_pointer, cppfuncsig_pointer, arg_indexes=None):
        self._cfuncsig_pointer = cfuncsig_pointer
        self._cppfuncsig_pointer = cppfuncsig_pointer
        self._arg_indexes = arg_indexes

    @property
    def cfuncsig_pointer(self):
        return self._cfuncsig_pointer

    @property
    def cppfuncsig_pointer(self):
        return self._cppfuncsig_pointer

    @property
    def arg_indexes(self):
        return self._arg_indexes
    