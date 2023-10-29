import importlib
from enum import Enum, auto
from accessor_data import AccessorData
import global_func_conv
import global_func_funcsig_conv
import constructor_method_conv
import constructor_method_funcsig_conv
import destructor_method_conv
import destructor_method_funcsig_conv
import inherited_method_conv
import inherited_method_funcsig_conv
import private_method_conv
import private_method_funcsig_conv
import static_func_conv
import static_func_funcsig_conv
import copy

# Provides a collections of all converters required for converting a GRIB Accessor from C to C++
#
# Converters can be overridden to provide specialised behaviour - return the updated versions
# here

# The Converter enum constrains the members of the map that we support
class Converter(Enum):
    GLOBAL_FUNC = auto()
    GLOBAL_FUNC_FUNCSIG = auto()
    CONSTRUCTOR_METHOD = auto()
    CONSTRUCTOR_METHOD_FUNCSIG = auto()
    DESTRUCTOR_METHOD = auto()
    DESTRUCTOR_METHOD_FUNCSIG = auto()
    INHERITED_METHOD = auto()
    INHERITED_METHOD_FUNCSIG = auto()
    PRIVATE_METHOD = auto()
    PRIVATE_METHOD_FUNCSIG = auto()
    STATIC_FUNC = auto()
    STATIC_FUNC_FUNCSIG = auto()

default_converters = {
    Converter.GLOBAL_FUNC:                global_func_conv.GlobalFunctionConverter,
    Converter.GLOBAL_FUNC_FUNCSIG:        global_func_funcsig_conv.GlobalFunctionFuncSigConverter,
    Converter.CONSTRUCTOR_METHOD:         constructor_method_conv.ConstructorMethodConverter,
    Converter.CONSTRUCTOR_METHOD_FUNCSIG: constructor_method_funcsig_conv.ConstructorMethodFuncSigConverter,
    Converter.DESTRUCTOR_METHOD:          destructor_method_conv.DestructorMethodConverter,
    Converter.DESTRUCTOR_METHOD_FUNCSIG:  destructor_method_funcsig_conv.DestructorMethodFuncSigConverter,
    Converter.INHERITED_METHOD:           inherited_method_conv.InheritedMethodConverter,
    Converter.INHERITED_METHOD_FUNCSIG:   inherited_method_funcsig_conv.InheritedMethodFuncSigConverter,
    Converter.PRIVATE_METHOD:             private_method_conv.PrivateMethodConverter,
    Converter.PRIVATE_METHOD_FUNCSIG:     private_method_funcsig_conv.PrivateMethodFuncSigConverter,
    Converter.STATIC_FUNC:                static_func_conv.StaticFunctionConverter,
    Converter.STATIC_FUNC_FUNCSIG:        static_func_funcsig_conv.StaticFunctionFuncSigConverter
}

def converters_for(accessor_name):
    converters = copy.deepcopy(default_converters)

    try:
        m = importlib.import_module(f"converters.{accessor_name}")

        if "update_converters" in dir(m):
            converters = m.update_converters(converters)
            assert converters, f"Converter for {accessor_name} is None"
    except ModuleNotFoundError:
        pass

    return converters
