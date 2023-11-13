from accessor_specific.default import AccessorSpecific

from converter_collection import Converter
import static_func_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
import arg
from funcsig_mapping import FuncSigMapping

class CodetableDataFuncSigConverter(static_func_funcsig_conv.StaticFunctionFuncSigConverter):
    func_conversions = [
        FuncSigMapping(FuncSig("int", "grib_inline_strcmp", [Arg("const char*", "a"), Arg("const char*", "b")]),
                       FuncSig(None, None, [None, None])),
        FuncSigMapping(FuncSig("int", "str_eq", [Arg("const char*", "a"), Arg("const char*", "b")]),
                       FuncSig(None, None, [None, None])),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class CodetableDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()
    
    def update_converters(self, converters):
        converters[Converter.STATIC_FUNC_FUNCSIG] = CodetableDataFuncSigConverter
        return converters
