
import debug
import default.cppcode as cppcode
import default.cnode_converter as cnode_converter
import transforms

# Convert a CCode object into a CppCode object, using the cconverter and derived classes as helpers

class CCodeConverter:
    def __init__(self, ccode_instance, manifest_class) -> None:
        self._ccode = ccode_instance
        self._manifest_class = manifest_class

    def convert(self):
        self.create_cpp_code()
        self.create_transforms()
        self.convert_global_declarations()

        return self._cppcode
    
    def create_cpp_code(self):
        self._cppcode = self._manifest_class.CPP_CODE()

    def create_transforms(self):
        self._transforms = transforms.Transforms()

    def convert_global_declarations(self):
        global_decl_converter = self._manifest_class.CNODE_CONVERTER(self._manifest_class)
        lines = global_decl_converter.convert(self._ccode.global_declarations, self._transforms)
        for line in lines:
            debug.line("convert_global_declarations", f"line=[{line}]")
