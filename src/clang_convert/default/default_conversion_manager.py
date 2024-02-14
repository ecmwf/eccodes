import ast_object.cfile_parser as cfile_parser
import default.default_ast_code_converter as default_ast_code_converter
import default.default_ccode_converter as default_ccode_converter
import default.default_cpp_writer as default_cpp_writer
import utils.debug as debug

# The main class for parsing a collection of C files and converting them to C++
#
# Call this to orchestrate everything!
#
# Override for specialised behaviour, e.g. GribAccessor
class DefaultConversionManager:
    def __init__(self, output_path, cli_logger):
        self._output_path = output_path + self.conversion_specific_path
        self._cli_logger = cli_logger

    # Conversion-specific : begin ========================================================
    @property
    def conversion_specific_path(self):
        return ""

    @property
    def ast_code_converter_class(self):
        return default_ast_code_converter.DefaultAstCodeConverter

    @property
    def ccode_converter_class(self):
        return default_ccode_converter.DefaultCCodeConverter    

    @property
    def cpp_writer_class(self):
        return default_cpp_writer.DefaultCppWriter

    @property
    def ignore_file_list(self):
        return []
    # Conversion-specific : end ==========================================================

    # This is the main entry point (should only need to call this!)
    # Returns a list of cppcode objects that can be written to files
    def convert(self, files):
        self._cli_logger.info("Output path=[%s]", self._output_path)
        self._files = files

        debug.line("convert", f"\n\n============================== [PHASE 1: Create AST code ] BEGIN ==============================\n")
        ast_code_list = self.create_ast_code_list()
        debug.line("convert", f"\n\n============================== [PHASE 1: Create AST code ] END   ==============================\n")

        debug.line("convert", f"\n\n============================== [PHASE 2: Convert AST to C] BEGIN ==============================\n")
        ccode_list = self.convert_ast_to_ccode(ast_code_list)
        debug.line("convert", f"\n\n============================== [PHASE 2: Convert AST to C] END   ==============================\n")

        debug.line("convert", f"\n\n============================== [PHASE 3: Convert C to C++] BEGIN ==============================\n")
        cppcode_list = self.convert_ccode_to_cppcode(ccode_list)
        debug.line("convert", f"\n\n============================== [PHASE 3: Convert C to C++] END   ==============================\n")

        debug.line("convert", f"\n\n============================== [PHASE 4: Write C++ files ] BEGIN ==============================\n")
        self.write_files(cppcode_list)
        debug.line("convert", f"\n\n============================== [PHASE 4: Write C++ files ] END   ==============================\n")

    def create_ast_code_list(self):
        full_ast_code_list = []
        full_file_list = []

        current_file_list = self._files
        while len(current_file_list) > 0:
            additional_ast_code_list, processed_files, additional_files = self.process_files(current_file_list)
            full_file_list.extend(processed_files)
            full_ast_code_list.extend(additional_ast_code_list)

            current_file_list = []

            for file in additional_files:
                if file not in full_file_list:
                    self._cli_logger.info("Found parent file to process: %s", file)
                    current_file_list.append(file)

        return full_ast_code_list

    def process_files(self, files):
        additional_files = []
        parser = cfile_parser.CFileParser(self._cli_logger)
        ast_code_list, processed_files = parser.to_ast_code_list(files, self.ignore_file_list)

        for entry in ast_code_list:
            parent_file = self.get_parent_filename(entry)
            if parent_file and \
               parent_file not in self.ignore_file_list and \
               parent_file not in additional_files:
                additional_files.append(parent_file)

        return ast_code_list, processed_files, additional_files

    # Override as required...
    def get_parent_filename(self, ast_code_instance):
        return None

    def convert_ast_to_ccode(self, ast_code_list):
        ccode_list = []

        for ast_code_entry in ast_code_list:
            self._cli_logger.info("Converting AST to C for source %s", ast_code_entry.cfilename)
            converter = self.ast_code_converter_class(ast_code_entry)
            ccode_entry = converter.convert()
            ccode_list.append(ccode_entry)
        
        return ccode_list

    def convert_ccode_to_cppcode(self, ccode_list):
        cppcode_list = []

        for ccode_entry in ccode_list:
            self._cli_logger.info("Converting C to C++ for source %s", ccode_entry.cfilename)
            converter = self.ccode_converter_class(ccode_entry)
            cppcode_entry = converter.convert()
            cppcode_list.append(cppcode_entry)
        
        return cppcode_list
    
    def write_files(self, cppcode_list):
        cpp_writer = self.cpp_writer_class(self._output_path, self._cli_logger)
        cpp_writer.write_files(cppcode_list)

# Read by convert.py
CONVERSION_MANAGER_CLASS=DefaultConversionManager