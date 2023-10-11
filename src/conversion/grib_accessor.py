import debug
import member
import func
import global_func
import static_func
import constructor_method
import destructor_method
import inherited_method
import private_method
import os
import re

# Represents the C code parsed from a grib_accessor_class*cc file
class GribAccessor:
    def __init__(
        self,
        *,
        path,
        functions,
        includes,
        definitions
    ):
        assert "CLASS" in definitions, "No CLASS defined"
        assert "SUPER" in definitions, "No SUPER defined"
        assert "FACTORY" in definitions, "No FACTORY defined"

        self._name = self.extract_class_name(path)
        self._class = definitions["CLASS"][0]
        self._super = definitions["SUPER"][0]
        self._factory_name = definitions["FACTORY"][0]
        self._members = [member.Member.from_string(m) for m in definitions.get("MEMBERS", []) if m != ""]
        self._implements = definitions.get("IMPLEMENTS", [])
        self._body_includes = includes
        self.classify_functions(functions)

    class_name = "Accessor"
    type_name = "grib_accessor"

    @property
    def name(self):
        return self._name

    @property
    def class_name(self):
        return self._class

    @property
    def super(self):
        return self._super

    @property
    def factory_name(self):
        return self._factory_name

    @property
    def members(self):
        return self._members
    
    @property
    def global_function(self):
        return self._global_function

    @property
    def constructor(self):
        return self._constructor

    @property
    def destructor(self):
        return self._destructor if self._destructor else ""

    @property
    def inherited_methods(self):
        return self._inherited_methods

    @property
    def private_methods(self):
        return self._private_methods

    @property
    def static_functions(self):
        return self._static_functions

    def extract_class_name(self, path):
        path, _ = os.path.splitext(path)
        name = os.path.basename(path)
        return name

    # Sort all the passed in functions into the correct "bins"
    def classify_functions(self, functions):
        self._inherited_methods = []
        self._private_methods = []
        self._static_functions = []
        self._constructor = None
        self._destructor = None

        # Note: Using list(...) creates a static list of (key, value) pairs from the self._functions dict to iterate over
        # This allows for safe modifications (deletions in this case) to the dictionary within the loop.
        for name, f in list(functions.items()):
            if type(f) is global_func.GlobalFunction:
                self._global_function = functions.pop(name)
            elif type(f) is constructor_method.ConstructorMethod:
                self._constructor = functions.pop(name)
            elif type(f) is destructor_method.DestructorMethod:
                self._destructor = functions.pop(name)
            elif type(f) is inherited_method.InheritedMethod:
                self._inherited_methods.append(functions.pop(name))
            elif type(f) is private_method.PrivateMethod:
                self._private_methods.append(functions.pop(name))
            elif type(f) is static_func.StaticFunction:
                self._static_functions.append(functions.pop(name))

        # Check we processed all functions!
        assert len(functions) == 0, sorted(functions.keys())

# Factory to create the correct Function based on the supplied definitions
def create_cfunction(func_sig, definitions):
    cfunction = None

    if func_sig.name == func.global_function_name:
        cfunction = global_func.GlobalFunction(func_sig)
    elif func_sig.name == "init":
        cfunction = constructor_method.ConstructorMethod(func_sig)
    elif func_sig.name == "destroy":
        cfunction = destructor_method.DestructorMethod(func_sig)
    elif func_sig.name in definitions.get("IMPLEMENTS", []):
        cfunction = inherited_method.InheritedMethod(func_sig)

    if not cfunction:
        # If any arg starts with a "ptr type name", then it's a private method (as we've already extracted inherited functions)
        for arg in func_sig.args:
            if re.search(r"grib_accessor(\w*)?\*", arg.type):
                cfunction = private_method.PrivateMethod(func_sig)
    
    if not cfunction:
        cfunction = static_func.StaticFunction(func_sig)
   
    return cfunction
