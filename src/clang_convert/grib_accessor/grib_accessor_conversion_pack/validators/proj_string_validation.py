
import utils.debug as debug
import grib_accessor.grib_accessor_conversion_pack.grib_accessor_conversion_validation as grib_accessor_conversion_validation
import re
import code_object.macro_instantation as macro_instantation
import code_object.code_objects as code_objects
import code_object.literal as literal

class ProjStringValidation(grib_accessor_conversion_validation.GribAccessorConversionValidation):

    def validate_macro_instantiation(self, cmacro_instantiation, cppmacro_instantiation):
        cpp_macro_text = cppmacro_instantiation.as_string()
        if cpp_macro_text.startswith("NUMBER"):
            new_code = re.sub(r"NUMBER\(([^\)]+)\)", rf"\1.size()", cpp_macro_text)
            new_code_object = literal.Literal(new_code)
            debug.line("validate_macro_instantiation", f"Replaced [{cpp_macro_text}] with [{new_code_object.as_string()}]")

            return new_code_object

        return super().validate_macro_instantiation(cmacro_instantiation, cppmacro_instantiation)
