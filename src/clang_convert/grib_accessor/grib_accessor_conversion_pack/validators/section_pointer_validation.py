
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_conversion_validation as grib_accessor_conversion_validation
import code_object_converter.conversion_pack.arg_utils as arg_utils

class SectionPointerValidation(grib_accessor_conversion_validation.GribAccessorConversionValidation):

    def validate_cast_expression(self, ccast_expression, cppcast_expression):
        if self._conversion_data.current_cfuncname == "init":
            # We don't need either cast expression in the constructor, so remove them!
            debug.line("validate_cast_expression", f"[init()] Removing cast expression: [{debug.as_debug_string(cppcast_expression)}]->[{debug.as_debug_string(cppcast_expression.expression)}]")
            return cppcast_expression.expression

        return super().validate_cast_expression(ccast_expression, cppcast_expression)