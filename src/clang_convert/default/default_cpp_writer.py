
from jinja2 import Environment, FileSystemLoader, StrictUndefined
import debug
import os

# Write the supplied cppcode objects to disk
class DefaultCppWriter:
    def __init__(self, target_path, cli_logger):
        self._target_path = target_path
        self._cli_logger = cli_logger
        
        # Override these as required
        self._j2_template_path = f"{os.path.dirname(__file__)}/j2"
        self._header_template = "default_header.h.j2"
        self._source_template = "default_source.cc.j2"
        self._cmakelists_template = "default_cmakelists.txt.j2"

    def setup_jinja(self):
        debug.line("__init__", f"j2 template path=[{self._j2_template_path}]")

        self._env = Environment(
            loader=FileSystemLoader(self._j2_template_path),
            undefined=StrictUndefined,
            )

    # Main entry point to write out the files
    def write_files(self, cppcode_entries):
        self.setup_jinja()
        
        for cppcode_entry in cppcode_entries:
            self.write_header_file(cppcode_entry)
            self.write_source_file(cppcode_entry)

        filenames = [cpp.file_stem for cpp in cppcode_entries]
        self.write_makefile(filenames)

    def write_header_file(self, cppcode):
        file_stem = cppcode.file_stem
        debug.line("write_header_file", f"file_stem = {file_stem}")
        template = self._env.get_template(self._header_template)
        self.save(file_stem, "h", template.render(c=cppcode))

    def write_source_file(self, cppcode):
        file_stem = cppcode.file_stem
        debug.line("write_source_file", f"file_stem = {file_stem}")
        template = self._env.get_template(self._source_template)
        self.save(file_stem, "cc", template.render(c=cppcode))

    def write_makefile(self, filenames):
        template = self._env.get_template(self._cmakelists_template)
        content = template.render(c=filenames)

        target = os.path.join(self._target_path, "CMakeLists.txt")
        self._cli_logger.info("Writing %s", target)

        os.makedirs(os.path.dirname(target), exist_ok=True)
        with open(target, "w") as f:
            f.write(content)

    def save(self, file_stem, ext, content):
        target = os.path.join(self._target_path, f"{file_stem}.{ext}")
        self._cli_logger.info("Writing %s", target)

        tmp = os.path.join(self._target_path, f"{file_stem}-tmp.{ext}")
        os.makedirs(os.path.dirname(target), exist_ok=True)

        with open(tmp, "w") as f:
            f.write(content)

        ret = os.system(f"clang-format -i {tmp}")
        assert ret == 0

        # So we don't recompile everything
        if os.path.exists(target):
            with open(target) as f:
                old = f.read()
            with open(tmp) as f:
                new = f.read()
            if old == new:
                self._cli_logger.info("No change")
                os.unlink(tmp)
                return

        self._cli_logger.info("Updating %s", target)
        os.rename(tmp, target)

# Read from convert.py to get the correct class name
CPP_WRITER_CLASS=DefaultCppWriter