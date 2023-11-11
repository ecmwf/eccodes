from accessor_specific.default import AccessorSpecific

from converter_collection import Converter
import static_func_funcsig_conv
from funcsig import FuncSig
from arg_indexes import ArgIndexes
from arg import Arg
import arg
from funcsig_mapping import FuncSigMapping

class BufrDataArrayDataFuncSigConverter(static_func_funcsig_conv.StaticFunctionFuncSigConverter):
    func_conversions = [
        # static int check_overridden_reference_values(const grib_context* c, long* refValList, size_t refValListSize, int numBits)
        FuncSigMapping(FuncSig("int", "check_overridden_reference_values", [Arg("const grib_context*", "c"), Arg("long*", "refValList"), Arg("size_t", "refValListSize"), Arg("int", "numBits")]),
                       FuncSig("GribStatus", "checkOverriddenReferenceValues", [None, Arg("std::vector<long>", "refValList"), None, Arg("int", "numBits")]),
                       ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
        # static grib_accessor* create_attribute_variable(const char* name, grib_section* section, int type, char* sval, double dval, long lval, unsigned long flags)
        FuncSigMapping(FuncSig("grib_accessor*", "create_attribute_variable", [Arg("const char*", "name"), Arg("grib_section*", "section"), Arg("int", "type"), Arg("char*", "sval"), Arg("double", "dval"), Arg("long", "lval"), Arg("unsigned long", "flags")]),
                       FuncSig("AccessorName", "createAttributeVariable", [Arg("std::string&", "name"), None, Arg("GribType", "type"), Arg("std::string&", "sval"), Arg("double", "dval"), Arg("long", "lval"), Arg("unsigned long", "flags")])),

    ]

    def __init__(self, cfuncsig):
        super().__init__(cfuncsig)
        self._conversions.extend(self.func_conversions)

class BufrDataArrayDataAccessorSpecific(AccessorSpecific):
    def __init__(self) -> None:
        super().__init__()

        self._custom_arg_transforms = {
            arg.Arg("const char*","dataKeysName") : arg.Arg("AccessorName","dataKeysName"),
            }

    def update_converters(self, converters):
        converters[Converter.STATIC_FUNC_FUNCSIG] = BufrDataArrayDataFuncSigConverter
        return converters
