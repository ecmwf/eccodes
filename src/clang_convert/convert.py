#!/usr/bin/env python3

import grib_accessor.grib_accessor_converter

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
        converter_type = "default"
    else:
        converter_type = ARGS.type

    converter_lib_name = f"{converter_type}.{converter_type}_converter"
    LOG.info("Type=[%s] converter_lib_name=[%s]", converter_type, converter_lib_name)

    try:
        converter_lib = importlib.import_module(converter_lib_name)
        converter = converter_lib.CONVERTER_CLASS(LOG)
        converter.convert_files(ARGS.path)
    except ModuleNotFoundError:
        LOG.error("Import failed, converter_lib_name=[%s]", converter_lib_name)
        exit()

if __name__ == "__main__":
    main()
