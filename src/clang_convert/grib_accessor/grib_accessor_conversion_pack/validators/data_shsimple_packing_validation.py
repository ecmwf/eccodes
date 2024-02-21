
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_conversion_validation as grib_accessor_conversion_validation
import code_object_converter.conversion_pack.arg_utils as arg_utils
import code_object.literal as literal
from code_object_converter.conversion_utils import as_commented_out_code

class DataShsimplePackingValidation(grib_accessor_conversion_validation.GribAccessorConversionValidation):

    def validate_unary_operation(self, cunary_operation, cppunary_operation):
        if self._conversion_data.current_cfuncname == "pack_double":
            if cppunary_operation.as_string() == "doubleValues++;":
                return as_commented_out_code(cppunary_operation, f"Removed invalid unary operation - will handle in next function call")

        return super().validate_unary_operation(cunary_operation, cppunary_operation)

    def validate_function_call(self, cfunction_call, cppfunction_call):

        if self._conversion_data.current_cfuncname == "pack_double":
            if cppfunction_call.name == "gribSetDouble":
                if cppfunction_call.args[0].as_string() == "codedValues_":
                    cppfunction_call.args[1] = literal.Literal("doubleValues[1]")

        return super().validate_function_call(cfunction_call, cppfunction_call)