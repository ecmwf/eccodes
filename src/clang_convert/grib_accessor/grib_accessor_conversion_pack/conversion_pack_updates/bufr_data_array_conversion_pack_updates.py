
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.conversion_pack_updates.base_conversion_pack_updates as base_conversion_pack_updates
from code_object.arg import Arg
from code_object_converter.conversion_pack.funcsig_mapping import FuncSigMapping
from code_object_converter.conversion_pack.arg_indexes import ArgIndexes
from code_object.funcsig import FuncSig
from code_object.code_interface import NONE_VALUE

class BufrDataArrayConversionPackUpdates(base_conversion_pack_updates.BaseConversionPackUpdates):
    def __init__(self) -> None:
        super().__init__()

        self._funcsig_mappings.extend([
            # static int check_overridden_reference_values(const grib_context* c, long* refValList, size_t refValListSize, int numBits)
            FuncSigMapping(FuncSig("int", "check_overridden_reference_values", [Arg("const grib_context*", "c"), Arg("long*", "refValList"), Arg("size_t", "refValListSize"), Arg("int", "numBits")]),
                        FuncSig("GribStatus", "checkOverriddenReferenceValues", [NONE_VALUE, Arg("std::vector<long>", "refValList"), NONE_VALUE, Arg("int", "numBits")]),
                        ArgIndexes(cbuffer=1, clength=2, cpp_container=1)),
            # static grib_accessor* create_attribute_variable(const char* name, grib_section* section, int type, char* sval, double dval, long lval, unsigned long flags)
            FuncSigMapping(FuncSig("grib_accessor*", "create_attribute_variable", [Arg("const char*", "name"), Arg("grib_section*", "section"), Arg("int", "type"), Arg("char*", "sval"), Arg("double", "dval"), Arg("long", "lval"), Arg("unsigned long", "flags")]),
                        FuncSig("AccessorName", "createAttributeVariable", [Arg("std::string&", "name"), NONE_VALUE, Arg("GribType", "type"), Arg("std::string&", "sval"), Arg("double", "dval"), Arg("long", "lval"), Arg("unsigned long", "flags")])),
        ])

        self._all_function_arg_mappings.update({
            Arg("const char*","dataKeysName") : Arg("AccessorName","dataKeysName"),
        })
