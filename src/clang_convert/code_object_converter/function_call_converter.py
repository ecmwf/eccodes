
import utils.debug as debug
import code_object.function_call as function_call
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs
import code_object.arg as arg
from code_object.code_interface import NONE_VALUE
import code_object_converter.conversion_pack.arg_utils as arg_utils
import re
import code_object.array_access as array_access
import code_object.literal as literal

class FunctionCallConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, function_call.FunctionCall), f"Expected FunctionCall, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cfunction_call = self._ccode_object
        cppfunction_call = None

        debug.line("create_cpp_code_object", f"FunctionCallConverter [IN] cfunction_call=[{debug.as_debug_string(cfunction_call)}]")

        m = re.match(r".*->", cfunction_call.name)
        if m:
            # The function call is through a pointer. We'll remove the pointer part and convert the function
            # The pointer will be converted elsewhere...
            function_name_pointer = m.group(0)
            cfunction_call = function_call.FunctionCall(cfunction_call.name[m.end():], cfunction_call.args)
            debug.line("create_cpp_code_object", f"Converting [{debug.as_debug_string(self._ccode_object)}] without function name pointer [{function_name_pointer}] => [{debug.as_debug_string(cfunction_call)}]")
        else:
            function_name_pointer = ""

        # 1. Check if there is a function mapping defined
        mapping = conversion_pack.conversion_data.funcsig_mapping_for_cfuncname(cfunction_call.name)

        debug.line("create_cpp_code_object", f"FunctionCallConverter [-] name=[{cfunction_call.name}] mapping=[{mapping}]")

        if mapping:
            debug.line("create_cpp_code_object", f"FunctionCallConverter [1] mapping.cfuncsig=[{debug.as_debug_string(mapping.cfuncsig)}] -> mapping.cppfuncsig=[{debug.as_debug_string(mapping.cppfuncsig)}]")
            cpp_args = []
            for i, arg_entry in enumerate(mapping.cppfuncsig.args):
                debug.line("create_cpp_code_object", f"FunctionCallConverter [2] --> i=[{i}] arg_entry=[{debug.as_debug_string(arg_entry)}]")
                if arg_entry != NONE_VALUE:
                    # We expect this argument to be converted to the C++ version, however, some calls don't always convert as expected. 
                    # For example: [int grib_value_count(grib_accessor* a, long* count)] -> [GribStatus gribValueCount(AccessorPtr ptr, long& count)]
                    # This can be called with an accessor pointer if we're calling a different class instance, however if we're calling ourself it will be NONE_VALUE
                    # Therefore, if a value doesn't convert, we'll just use the C value (which we handle later)
                    cpp_arg_entry = conversion_funcs.convert_ccode_object(cfunction_call.args[i], conversion_pack)
                    if cpp_arg_entry == NONE_VALUE:
                        cpp_arg_entry = arg_entry
                        debug.line("create_cpp_code_object", f"FunctionCallConverter [3] carg=[{debug.as_debug_string(cfunction_call.args[i])}] has no C++ conversion (may be a <self> arg), setting cpp_arg_entry=[{debug.as_debug_string(cpp_arg_entry)}]")
                    else:
                        # Check if we have a container arg
                        cpp_arg_name = arg_utils.extract_name(cpp_arg_entry)
                        debug.line("create_cpp_code_object", f"FunctionCallConverter [4] cpp_arg_name=[{cpp_arg_name}] cpp_arg_entry=[{debug.as_debug_string(cpp_arg_entry)}]")

                        if cpp_arg_name:
                            cpp_container_arg = conversion_pack.container_utils.cname_to_cpp_container(cpp_arg_name, conversion_pack.conversion_data)
                            debug.line("create_cpp_code_object", f"FunctionCallConverter [5] cpp_container_arg=[{debug.as_debug_string(cpp_container_arg)}] for cpp_arg_name=[{cpp_arg_name}]")
                            if cpp_container_arg:
                                cpp_arg_entry = cpp_container_arg

                        cpparg = arg_utils.to_cpparg(cpp_arg_entry, conversion_pack.conversion_data)
                        if cpparg: 
                            if conversion_pack.conversion_data.is_container_type(cpparg.decl_spec.type) and not conversion_pack.conversion_data.is_container_type(mapping.cppfuncsig.args[i].decl_spec.type):
                                cpp_arg_entry = array_access.ArrayAccess(literal.Literal(cpparg.name), literal.Literal("0"))
                                debug.line("create_cpp_code_object", f"FunctionCallConverter [6] MISMATCH: cpparg=[{debug.as_debug_string(cpparg)}] is a container, C++ function arg=[{debug.as_debug_string(mapping.cppfuncsig.args[i])}] is NOT! Passing index 0, cpp_arg_entry=[{debug.as_debug_string(cpp_arg_entry)}]")

                    debug.line("create_cpp_code_object", f"ADDING cpp_arg_entry=[{debug.as_debug_string(cpp_arg_entry)}] arg_entry=[{debug.as_debug_string(arg_entry)}]")
                    cpp_args.append(cpp_arg_entry)

            cppfunction_call = function_call.FunctionCall(mapping.cppfuncsig.name, cpp_args)

            debug.line("create_cpp_code_object", f"cppfunction_call NOW EQUALS [{debug.as_debug_string(cppfunction_call)}]")

        else:
            debug.line("create_cpp_code_object", f"FunctionCallConverter [7]")
            # 2. Perform a manual conversion
            cpp_name = conversion_funcs.convert_ccode_object(cfunction_call.name, conversion_pack)
            cpp_args = []
            for arg_entry in self._ccode_object.args:
                cpp_arg_entry = conversion_funcs.convert_ccode_object(arg_entry, conversion_pack)

                if cpp_arg_entry and cpp_arg_entry != NONE_VALUE:
                    cpp_args.append(cpp_arg_entry)

            cppfunction_call = function_call.FunctionCall(cpp_name, cpp_args)

        # 3. Restore function name pointer before performing validation...
        if function_name_pointer:
            cppfunction_call = function_call.FunctionCall(function_name_pointer+cppfunction_call.name, cppfunction_call.args)
            debug.line("create_cpp_code_object", f"Restoring function_name_pointer=[{function_name_pointer}] --> [{debug.as_debug_string(cppfunction_call)}]")

        # 4. Apply validation (and special handling)
        updated_cppfunction_call = conversion_pack.conversion_validation.validate_function_call(cfunction_call, cppfunction_call)

        # 5. Add the function call to the conversion data in case we need to process it later (e.g. for using C::x declarations)
        # Only if function_call type!
        if isinstance(updated_cppfunction_call, function_call.FunctionCall):
            conversion_pack.conversion_data.add_cppfunction_call(updated_cppfunction_call)



        return updated_cppfunction_call
