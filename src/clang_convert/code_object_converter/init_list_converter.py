
import utils.debug as debug
import code_object.init_list as init_list
import code_object_converter.code_interface_converter as code_interface_converter
import code_object_converter.conversion_funcs as conversion_funcs

class InitListConverter(code_interface_converter.CodeInterfaceConverter):
    def __init__(self, ccode_object) -> None:
        super().__init__(ccode_object)
        assert isinstance(ccode_object, init_list.InitList), f"Expected InitList, got type=[{type(ccode_object)}]"

    def create_cpp_code_object(self, conversion_pack):
        cpp_decl_spec = conversion_funcs.convert_ccode_object(self._ccode_object.decl_spec, conversion_pack)
        cpp_init_list = init_list.InitList(cpp_decl_spec)

        for entry in self._ccode_object.entries:
            cpp_entry = conversion_funcs.convert_ccode_object(entry, conversion_pack)
            cpp_init_list.add_entry(cpp_entry)

        return conversion_pack.conversion_validation.validate_init_list(self._ccode_object, cpp_init_list)
