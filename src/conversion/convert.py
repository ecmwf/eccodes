#!/usr/bin/env python3

import argparse
import os
import re
import logging

from jinja2 import Environment, FileSystemLoader, StrictUndefined

import debug
from global_func_funcsig_conv import global_cfuncsig
import funcsig
import grib_accessor
from grib_accessor_conv import GribAccessorConverter

import cProfile
import pstats
from pstats import SortKey

LOG = logging.getLogger(__name__)

parser = argparse.ArgumentParser()
parser.add_argument("--templates", default="conversion/j2")
parser.add_argument("--target", default="cpp/converted")
parser.add_argument("--debug", action="store_true")
parser.add_argument("path", nargs="+")

ARGS = parser.parse_args()

logging.basicConfig(
    format="%(asctime)s %(levelname)s %(message)s",
    level=logging.DEBUG if ARGS.debug else logging.INFO,
    datefmt="%Y-%m-%d %H:%M:%S",
)

env = Environment(
    loader=FileSystemLoader(ARGS.templates),
    undefined=StrictUndefined,
)

def count_parentheses(text):
    open_paren = 0
    close_paren = 0
    in_single_quote = False
    in_double_quote = False
    escape_char = False

    for char in text:
        # Skip if the current character is escaped
        if escape_char:
            escape_char = False
            continue

        # If it's the escape character and we're in a quote, skip the next character
        if char == '\\' and (in_single_quote or in_double_quote):
            escape_char = True
            continue

        # Toggle the single quote flag if we're not in double quotes
        if char == "'" and not in_double_quote:
            in_single_quote = not in_single_quote

        # Toggle the double quote flag if we're not in single quotes
        elif char == '"' and not in_single_quote:
            in_double_quote = not in_double_quote

        # Count the parentheses if we're not in any quote
        elif not in_single_quote and not in_double_quote:
            if char == '(':
                open_paren += 1
            elif char == ')':
                close_paren += 1

    return open_paren, close_paren

def parse_file(path):
    in_definition = False
    in_implementation = False
    in_function = False
    includes = []
    template = None
    depth = 0

    definitions = {}
    functions = {}

    # Create a global function for storing global vars etc
    global_function = functions[global_cfuncsig.name] = grib_accessor.create_cfunction(
        global_cfuncsig,
        definitions
    )

    LOG.info("Parsing %s", path)

    # Some function calls are split over multiple lines, which causes issues when 
    # parsing, so we combine these into a single long line
    multiline = ""
    function_start_re     = r"\b[^\(\s]*\("
    comment_or_space_re   = r"((\s*)|(\s*/\*.*\*/\s*)|(\s*//.*))"
    function_continues_re = r"[,\(\"]" + comment_or_space_re + "$"
    open_paren_count = 0
    close_paren_count = 0
    forward_declarations = []

    f = open(path, "r")
    for line in f:

        # Multiline function parsing - start
        if not multiline and re.search(rf"{function_start_re}", line):
            open_paren_count, close_paren_count = count_parentheses(line)
            if open_paren_count > close_paren_count:
                multiline = line
                continue
            else:
                open_paren_count = close_paren_count = 0

        if multiline:
            new_open, new_close = count_parentheses(line)
            open_paren_count  += new_open
            close_paren_count += new_close

            if open_paren_count > close_paren_count:
                multiline += line.lstrip()
                continue
            elif open_paren_count == close_paren_count:
                multiline = multiline.replace("\n", "")
                line = multiline + line.lstrip()
                debug.line("parse_file", f"multiline=[{line}]")
                multiline = ""
                open_paren_count = close_paren_count = 0
            else:
                assert False, f"open_paren_count [{open_paren_count}] < close_paren_count [{close_paren_count}]"

        # Multiline function parsing - end

        stripped_line = line.strip()
        line = line.rstrip()

        if stripped_line.startswith("START_CLASS_DEF"):
            in_definition = True
            continue

        if stripped_line.startswith("END_CLASS_DEF"):
            in_definition = False
            continue

        if stripped_line.startswith("/* START_CLASS_IMP */"):
            in_implementation = True
            # Discard any global lines captured before here...
            global_function.clear_lines()
            continue

        if stripped_line.startswith("/* END_CLASS_IMP */"):
            in_implementation = False
            continue

        if in_implementation:
            m = re.match(r"\s*\"(\w+)\",\s+/\* name \*/", stripped_line)
            if m:
                definitions["FACTORY"] = [m.group(1)]

            continue

        if in_definition:
            if stripped_line.strip() == "":
                continue
            bits = [s.strip() for s in re.split(r"[=;]+", stripped_line)]
            try:
                what = bits[0]
                definitions.setdefault(what, [])
                definitions[what] += bits[1:]
            except KeyError:
                LOG.error(f"Unknown definition: {bits}")
                raise
            continue

        # Try and create a FuncSig from the line, if successful then it's a function definition!
        cfuncsig = funcsig.FuncSig.from_string(line)
        if cfuncsig:
            assert not in_function, line

            function_name = cfuncsig.name

            # If this is the first time we've seen this function name, add it to the global body 
            # as a forward declaration, so it is in the correct place for any other globals!
            #
            # Note: We just use the placeholder @FORWARD_DECLARATION:function_name here,
            #       when it is processed the placeholder will be used to check that it 
            #       is a valid static function: if not it will be deleted
            if not function_name in forward_declarations:
                forward_declarations.append(function_name)
                forward_declaration = f"@FORWARD_DECLARATION:{function_name}"
                debug.line("parse_file", f"[FORWARD DECLARATION] Adding [{forward_declaration}]")
                global_function.add_line(forward_declaration)

            # If this is an actual function declaration, then there's nothing to process
            if line.rstrip().endswith(");"):
                # Forward declaration
                continue


            in_function = True

            cfuncsig.template = template
            function = functions[function_name] = grib_accessor.create_cfunction(
                cfuncsig,
                definitions
            )
            depth = stripped_line.count("{") - stripped_line.count("}")
            assert depth >= 0, line
            LOG.debug("Start of function %s", function.name)
            continue

        if in_function:
            function.add_line(stripped_line)
            depth += stripped_line.count("{")
            depth -= stripped_line.count("}")
            assert depth >= 0, line
            if depth == 0 and not function.is_empty():
                LOG.debug("End of function %s", function.name)
                in_function = False
                template = None
                del function

            continue

        if stripped_line.startswith("template "):
            template = stripped_line
            continue

        global_function.add_line(line)

    # for line in f: [END]

    if definitions:
        gribAccessorImpl = grib_accessor.GribAccessor(
            path=path,
            functions=functions,
            includes=includes,
            definitions=definitions
        )
        return gribAccessorImpl
    
    assert False, "parse_file failed to return a value for: {path}"


def save(cpp_obj, ext, content):
    target = os.path.join(ARGS.target, *cpp_obj.namespaces, f"{cpp_obj.name}.{ext}")
    LOG.info("Writing %s", target)

    tmp = os.path.join(ARGS.target, *cpp_obj.namespaces, f"{cpp_obj.name}-tmp.{ext}")
    os.makedirs(os.path.dirname(target), exist_ok=True)

    with open(tmp, "w") as f:
        f.write(content)

    ret = os.system(f"clang-format -i {tmp}")
    assert ret == 0

    # So we don't recomplie everything
    if os.path.exists(target):
        with open(target) as f:
            old = f.read()
        with open(tmp) as f:
            new = f.read()
        if old == new:
            LOG.info("No change")
            os.unlink(tmp)
            return

    LOG.info("Updating %s", target)
    os.rename(tmp, target)

def write_header_file(cpp_obj):
    debug.line("write_header_file", f"Class = {cpp_obj.name}")
    template = env.get_template(f"accessorData.h.j2")
    save(cpp_obj, "h", template.render(c=cpp_obj))

def write_source_file(cpp_obj):
    debug.line("write_source_file", f"Class = {cpp_obj.name}")
    # Beware of this: https://github.com/pallets/jinja/issues/604
    template = env.get_template(f"accessorData.cc.j2")
    save(cpp_obj, "cc", template.render(c=cpp_obj))

def write_makefile(class_list):
    template = env.get_template("CMakeLists.txt.j2")
    content = template.render(c=class_list)

    target = os.path.join(ARGS.target, "CMakeLists.txt")
    LOG.info("Writing %s", target)

    os.makedirs(os.path.dirname(target), exist_ok=True)
    with open(target, "w") as f:
        f.write(content)

ignore_files = ["grib_accessor_class_gen.cc"]

def convert_grib_accessors():
    grib_accessors = {}
    accessor_name_list = []

    for a in ARGS.path:
        if a in ignore_files:
            LOG.info("Ignoring file %s", a)
        else:
            gribAccessorInst = parse_file(a)
            if gribAccessorInst is not None:
                grib_accessors[gribAccessorInst._name] = gribAccessorInst

    for gribAccessorInst in grib_accessors.values():
        converter = GribAccessorConverter(gribAccessorInst)
        accessorDataImpl = converter.to_accessor_data(grib_accessors)
        accessor_name_list.append(accessorDataImpl.name)
        write_header_file(accessorDataImpl)
        write_source_file(accessorDataImpl)

    write_makefile(accessor_name_list)

def main():
    convert_grib_accessors()
    
    #cProfile.run('convert_grib_accessors()', 'output.prof')
    #p = pstats.Stats('output.prof')
    #p.sort_stats(SortKey.CUMULATIVE).print_stats(10)

if __name__ == "__main__":
    main()
