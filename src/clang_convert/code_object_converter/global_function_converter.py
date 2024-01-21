
import utils.debug as debug
import code_object.global_function as global_function
import code_object_converter.function_converter as function_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.code_objects as code_objects
import code_object.funcsig as funcsig

class GlobalFunctionConverter(function_converter.FunctionConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, global_function.GlobalFunction), f"Expected GlobalFunction, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_data):
        self._conversion_data = conversion_data
        cpp_body = code_objects.CodeObjects()

        for entry in self._ccode_object.body.code_objects:
            if isinstance(entry, funcsig.FuncSig) and conversion_data.is_member_function(entry.name):
                debug.line("create_cpp_code_object", f"Ignoring member function name=[{entry.name}]")
                continue

            cpp_code_obj = conversion_funcs.convert_ccode_object(entry, conversion_data)
            cpp_body.add_code_object(cpp_code_obj)

        return global_function.GlobalFunction(cpp_body)
  
