
import debug
import default.default_ccode_converter as default_ccode_converter
import code_object.cppfunction as cppfunction

# Converter for grib_accessor_ccode classes...
class GribAccessorCCodeConverter(default_ccode_converter.DefaultCCodeConverter):
    def __init__(self, ccode_instance) -> None:
        super().__init__(ccode_instance)

    # TODO when the class is available...
    @property
    def cpp_code_class(self):
        return super().cpp_code_class

