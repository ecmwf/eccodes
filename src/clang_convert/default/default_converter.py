import default.default_cfile_parser as default_cfile_parser
import default.default_ccode_converter as default_ccode_converter

# The main class for parsing a collection of C files and converting them to C++

class DefaultConverter:
    def __init__(self, cli_logger):
        self._cli_logger = cli_logger

    # Override to set the correct parser class as required by derived classes
    @property
    def cfile_parser_class(self):
        return default_cfile_parser.DefaultCFileParser

    # Override to set the correct CCode Converter class as required by derived classes
    @property
    def ccode_converter_class(self):
        return default_ccode_converter.DefaultCCodeConverter

    # Override to set the correct files to be ignored by derived classes
    @property
    def ignore_file_list(self):
        return []

    # This is the main entry point (should only need to call this!)
    # Returns a list of cppcode objects that can be written to files
    def convert_files(self, files):

        cppcode_entries = []
        for ccode_entry in self.parse_files(files):
            self._cli_logger.info("Converting C to C++ for source %s", ccode_entry.cfilename)
            ccode_conv = self.ccode_converter_class(ccode_entry)
            cppcode_entry = ccode_conv.convert()
            cppcode_entries.append(cppcode_entry)
        
        return cppcode_entries

    # Called by convert_files()...
    def parse_files(self, files):
        file_parse_list = []

        if files[0].endswith("input_files"):
            f = open(files[0], "r")
            for entry in f:
                if not entry.startswith("#"):
                    file_parse_list.append(entry.rstrip())
        else:
            file_parse_list = files

        ccode_list = []

        for f in file_parse_list:
            if f in self.ignore_file_list:
                self._cli_logger.info("Ignoring C file %s", f)
            else:
                self._cli_logger.info("Parsing C file %s", f)
                cfile_parser = self.cfile_parser_class(f)
                c_code = cfile_parser.parse()
                ccode_list.append(c_code)
                #c_code.dump()

        # Parse any missing parents not listed
        self._cli_logger.info("Looking for missing parent files...")

        parent_ccode_list = []

        for ccode_entry in ccode_list:
            ccode_inst = ccode_entry
            parent_cfilename = ccode_inst.parent_cfilename

            while parent_cfilename and parent_cfilename not in self.ignore_file_list:
                if parent_cfilename in file_parse_list:
                    break
                else:
                    self._cli_logger.info("Parsing parent %s of C file %s", parent_cfilename, ccode_inst.cfilename)
                    file_parse_list.append(parent_cfilename)

                    cfile_parser = self.cfile_parser_class(f)
                    ccode_inst = cfile_parser.parse()
                    parent_ccode_list.append(ccode_inst)
                    parent_cfilename = ccode_inst.parent_cfilename

        self._cli_logger.info("Number of missing parent files parsed: %d", len(parent_ccode_list))
        ccode_list.extend(parent_ccode_list)

        return ccode_list

# Read from convert.py to get the correct class name
CONVERTER_CLASS=DefaultConverter