
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class CodetableConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._funcsig_mappings.extend([
            #FuncSigMapping(FuncSig("int", "str_eq", [Arg("const char*", "a"), Arg("const char*", "b")]),
            #            FuncSig("void", "strEq", [NONE_VALUE, NONE_VALUE])),
        ])
