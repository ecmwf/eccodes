
import utils.debug as debug
import default.default_conversion_pack.default_type_info as default_type_info

# Provides access to type conversions in a manner that can be overridden
class GribAccessorTypeInfo(default_type_info.DefaultTypeInfo):

    # Returns a list of names representing class instances, e.g. self->
    def class_instance_pointer_names(self):
         return super().class_instance_pointer_names() + ["a"]
    
    def cpp_container_types(self):
        return super().cpp_container_types() + [
            "AccessorDataBuffer",
        ]
