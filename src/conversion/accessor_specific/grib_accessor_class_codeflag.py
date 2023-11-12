from accessor_specific.default import AccessorSpecific

from converter_collection import Converter
import static_func_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
import arg
from funcsig_mapping import FuncSigMapping

class CodeflagDataFuncSigConverter(static_func_funcsig_conv.StaticFunctionFuncSigConverter):
    func_conversions = [
        FuncSigMapping(FuncSig("int", "test_bit", [Arg("long", "a"), Arg("long", "b")]),
                       FuncSig("int", "testBit", [Arg("long", "a"), Arg("long", "b")])),
    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class CodeflagDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()

        self._custom_member_arg_transforms = {
            arg.Arg("const char*","tablename") : arg.Arg("std::string","tablename"),
            }
        
        self._custom_final_line_transforms = {
            "if ((filename = gribContextFullDefsPath(fname)) == NULL) {" : "if ((filename = gribContextFullDefsPath(fname)) == \"\") {",
            }
    
    def update_converters(self, converters):
        converters[Converter.STATIC_FUNC_FUNCSIG] = CodeflagDataFuncSigConverter
        return converters

