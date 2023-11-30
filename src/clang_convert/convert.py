#!/usr/bin/env python3

# Must have libclang!
try:
    import clang.cindex
except ImportError:
    print("\nError: libclang is not installed. Please install it using 'pip install clang'.\n")
    exit(1)

import argparse
import logging
import importlib
import os

LOG = logging.getLogger(__name__)

parser = argparse.ArgumentParser()
parser.add_argument("--type", default="")
parser.add_argument("--target", default="cpp/converted")
parser.add_argument("--debug", action="store_true")
parser.add_argument("--libclang", default="/usr/lib/llvm-16/lib/libclang.so")
parser.add_argument("path", nargs="+")

ARGS = parser.parse_args()

logging.basicConfig(
    format="%(asctime)s %(levelname)s %(message)s",
    level=logging.DEBUG if ARGS.debug else logging.INFO,
    datefmt="%Y-%m-%d %H:%M:%S",
)

class CConverter:
    def __init__(self, manifest_lib_name):
        self._manifest_class = None
        self._ccode_list = []

        try:
            manifest_lib = importlib.import_module(manifest_lib_name)
            self._manifest_class = manifest_lib.MANIFEST_CLASS()
        except ModuleNotFoundError:
            LOG.error("Import failed, manifest_lib_name=[%s]", manifest_lib_name)
            exit()

    def parse_files(self, files):
        file_parse_list = []

        if files[0].endswith("input_files"):
            f = open(files[0], "r")
            for entry in f:
                if not entry.startswith("#"):
                    file_parse_list.append(entry.rstrip())
        else:
            file_parse_list = files

        self._ccode_list.clear()

        for f in file_parse_list:
            if f in self._manifest_class.ignore_file_list:
                LOG.info("Ignoring C file %s", f)
            else:
                LOG.info("Parsing C file %s", f)
                c_parser = self._manifest_class.CPARSER(f)
                c_code = c_parser.parse()
                self._ccode_list.append(c_code)
                #c_code.dump()

        # Parse any missing parents not listed
        LOG.info("Looking for missing parent files...")

        parent_ccode_list = []

        for ccode_entry in self._ccode_list:
            ccode_inst = ccode_entry
            parent_cfilename = ccode_inst.parent_cfilename

            while parent_cfilename and parent_cfilename not in self._manifest_class.ignore_file_list:
                if parent_cfilename in file_parse_list:
                    break
                else:
                    LOG.info("Parsing parent %s of C file %s", parent_cfilename, ccode_inst.cfilename)
                    file_parse_list.append(parent_cfilename)

                    c_parser = self._manifest_class.CPARSER(parent_cfilename)
                    ccode_inst = c_parser.parse()
                    parent_ccode_list.append(ccode_inst)
                    parent_cfilename = ccode_inst.parent_cfilename

        LOG.info("Number of missing parent files parsed: %d", len(parent_ccode_list))
        self._ccode_list.extend(parent_ccode_list)

    def convert_files(self):
        for ccode_entry in self._ccode_list:
            LOG.info("Converting C to C++ for source %s", ccode_entry.cfilename)
            ccode_conv = self._manifest_class.CCODE_CONVERTER(ccode_entry, self._manifest_class)
            ccode_conv.convert()

def main():
    if os.path.exists(ARGS.libclang):
        clang.cindex.Config.set_library_file(ARGS.libclang)
        LOG.info("libclang library file set to %s", ARGS.libclang)
    else:
        LOG.info("Could not set libclang library file to %s", ARGS.libclang)
        LOG.info("Use --libclang FILE to specify the file, e.g: convert.py --libclang /usr/lib/llvm-16/lib/libclang.so\n")
        exit(1)

    if not ARGS.type:
        LOG.info("C code type not supplied, using default.\n")
        LOG.info("Use --type to specify the type, e.g: convert.py --type grib_accessor\n")
        manifest_type = "default"
    else:
        manifest_type = ARGS.type

    manifest_lib_name = f"{manifest_type}.{manifest_type}_manifest"
    LOG.info("Type=[%s] manifest_lib_name=[%s]", manifest_type, manifest_lib_name)

    converter = CConverter(manifest_lib_name)
    converter.parse_files(ARGS.path)
    converter.convert_files()

if __name__ == "__main__":
    main()
