import ast_object.ast_code_creator as ast_code_creator

# The main class for parsing a collection of C files and converting them to a list of AstCode objects
class CFileParser:
    def __init__(self, cli_logger):
        self._cli_logger = cli_logger

    def to_ast_code_list(self, files, ignore_file_list):
        processed_files_list = []

        if files[0].endswith("input_files"):
            f = open(files[0], "r")
            for entry in f:
                if not entry.startswith("#"):
                    processed_files_list.append(entry.rstrip())
        else:
            processed_files_list = files

        ast_code_list = []

        for f in processed_files_list:
            if f in ignore_file_list:
                self._cli_logger.info("Ignoring C file %s", f)
            else:
                self._cli_logger.info("Parsing C file %s", f)
                creator = ast_code_creator.AstCodeCreator(f)
                ast_code = creator.create()
                ast_code_list.append(ast_code)

        return ast_code_list, processed_files_list
