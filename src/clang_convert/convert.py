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
parser.add_argument("--libclang", default="/usr/lib/llvm-17/lib/libclang.so")
parser.add_argument("path", nargs="+")

ARGS = parser.parse_args()

logging.basicConfig(
    format="%(asctime)s %(levelname)s %(message)s",
    level=logging.DEBUG if ARGS.debug else logging.INFO,
    datefmt="%Y-%m-%d %H:%M:%S",
)

# Call the appropriate conversion manager to convert and write the C++ files
def run_conversion(convert_type):
    conversion_manager_lib_name = f"{convert_type}.{convert_type}_conversion_manager"
    LOG.info("Type=[%s] conversion_manager_lib_name=[%s]", convert_type, conversion_manager_lib_name)

    try:
        conversion_manager_lib = importlib.import_module(conversion_manager_lib_name)
        conversion_manager = conversion_manager_lib.CONVERSION_MANAGER_CLASS(ARGS.target, LOG)
        return conversion_manager.convert(ARGS.path)
    except ModuleNotFoundError:
        LOG.error("Import failed, conversion_manager_lib_name=[%s]", conversion_manager_lib_name)
        exit()

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
        convert_type = "default"
    else:
        convert_type = ARGS.type

    run_conversion(convert_type)

if __name__ == "__main__":
    main()
