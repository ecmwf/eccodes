
import utils.debug as debug

# Provides access to type info in a manner that can be overridden
class DefaultTypeInfo:

    # Returns a list of names representing class instances, e.g. self->
    def class_instance_pointer_names(self):
         return ["self", "this"]
    
    def cpp_container_types(self):
        return [
            "std::string",
            "std::array",
            "std::vector",
            "std::map",
        ]
