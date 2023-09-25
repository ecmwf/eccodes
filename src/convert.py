#!/usr/bin/env python3

import argparse
import importlib
import os
import re
from collections import defaultdict
import logging

from jinja2 import Environment, FileSystemLoader, StrictUndefined

import conversion.convert_data as convert_data

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

func_pad = 30
debug_line_enabled = True
debug_filter = []

def debug_line(func, text):
    if not debug_line_enabled:
        return
    
    if debug_filter and not func in debug_filter:
        return
    
    if len(func) > func_pad:
        print(f">>>>>")
        print(f">>>>> PADDING ({func_pad}) TOO SMALL FOR FUNC NAME: {func} - size={len(func)}")
        print(f">>>>>")

    print(f"{func:{func_pad}}: {text}")


def transform_variable_name(name):
    name_parts = name.split("_")

    if len(name_parts) > 1:
        name = name_parts[0] + "".join(x.capitalize() for x in name_parts[1:])
    
    return name[0].lower() + name[1:]

def transform_function_name(name):
    name = re.sub(rf"^[gs]et_", f"", name)
    return transform_variable_name(name)

def transform_class_name(name):
    name = transform_variable_name(name)
    return name[0].upper() + name[1:]

global_function_name = "Global"

common_includes = [
    "AccessorUtils/AccessorProxy.h", 
    "AccessorFactory.h", 
    "AccessorUtils/ConversionHelper.h",
    "AccessorUtils/GribUtils.h"
    ]

class Member:
    def __init__(self, s) -> None:
        bits = s.split()
        self.type = " ".join(bits[:-1])

        if "[" in bits[-1]:
            self.cname, self._array = bits[-1].split("[")
            self._array = "[" + self._array
        else:
            self.cname = bits[-1]
            self._array = ""

        self.name = transform_variable_name(self.cname) + "_"

        if self.name[0] == "*":
            self.name = self.name[1:]
            self.type += "*"

        # We'll assume "const char*" means this variable refers to another accessor...
        if self.type == "const char*":
            self.type = "AccessorName"
            self.default_value = "{\"\"}"
        else:
            self.default_value = ""

        self._mutable = False

    @property
    def mutable(self):
        return "mutable" if self._mutable else ""

    @property
    def array(self):
        return self._array

class Arg:
    def __init__(self, name, type) -> None:
        self.name = name
        self.type = type

        if self.name[0] == "*":
            self.name = self.name[1:]
            self.type += "*"

class Function:
    def __init__(self, name, return_type, args, template=None) -> None:
        self._name = name
        self._return_type = return_type
        self._template = template
        self._cargs = []
        self._lines = []

        # args may be empty...
        if isinstance(args, str):
            for arg in [a.strip() for a in args.split(",")]:
                if not arg:
                    continue
                bits = arg.split()
                type = " ".join(bits[:-1])
                name = bits[-1]
                carg = Arg(name, type)
                self._cargs.append(carg)

    def update_lines(self, lines):
        self._lines = lines

    def is_empty(self):
        return len(self._lines) == 0

    def clear_lines(self):
        self._lines.clear()

    def add_line(self, line):
        self._lines.append(line)

    @property
    def name(self):
        return self._name

    @property
    def args(self):
        return self._cargs

    @property
    def return_type(self):
        return self._return_type

    @property
    def const(self):
        return ""

    @property
    def template(self):
        return "" if self._template is None else self._template

    @property
    def code(self):
        if self._lines:
            assert self._lines[0].strip() == "{", "\n".join(self._lines)
            assert self._lines[-1].strip() == "}", "\n".join(self._lines)
        return self._lines[1:-1]

    def cannot_convert(self):
        self._lines.insert(1, "#ifdef CANNOT_CONVERT_CODE")
        self._lines.insert(-1, "#endif")
        if self._name != "destroy":
            self._lines.insert(-1, "throw AccessorException(GRIB_NOT_IMPLEMENTED);")


# Note - these substitutions are applied in the order defined below, so dependencies
#        can be used if required...
basic_function_substitutions = {
    # grib_ functions
    # Note: 1. We treat e.g. grib_get_long and grib_get_long_internal the same...
    #       2. The first argument (h) may already be stripped so we optionally match it
    #       3. The second argument may be a string literal, which needs to convert to an AccessorName object
    #
    # First, let's convert the second argument to an AccessorName if required
    r"\b(grib_get_\w+)(_internal)?\(\s*(h\s*,\s*)?\s*(\".*\")": r"\1\2(\3AccessorName(\4)",
    # Now, complete the conversion - note we remove any references
    r"\b([\w\s]+)\s*=\s*grib_get_long(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = toLong(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_get_double(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*)\s*\)": r"\1 = toDouble(\4, \5)",
    r"\b([\w\s]+)\s*=\s*grib_get_string(_internal)?\(\s*(h\s*,\s*)?\s*(.*),\s*&?(.*),\s*(.*)\s*\)": r"\1 = toString(\4, \5)",

    # C functions
    r"\bstrcmp\((.*),\s*(.*)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
    r"\bstrlen\(\s*(.*)\s*\)": r"\1.size()",
    # snprintf substitutions can span multiple lines, but we only need to match to the start of the format string...
    r"\bsnprintf\(([\w\d]+),\s*(\d+)\s*,\s*(\")": r"\1 = fmtString(\2,\3"
}

# NOTE - Don't access this dictionary directly - use cpp_func_body_arg_type_for() below...
carg_type_transforms = {
    "grib_accessor*"    : None,
    "grib_handle*"      : None,
    "char*"             : "std::string"
}


# Returns the equivalent C++ type, which could be None
def cpp_func_body_arg_type_for(ctype):
    for k, v in carg_type_transforms.items():
        if k == ctype:
            return v
    
    # Return None for grib_accessor_*
    m = re.match(r"grib_accessor_", ctype)
    if m:
        return None

    # Pointer types
    m = re.match(r"([\w\d]+)\*", ctype)
    if m:
        return m.group(1)

    return ctype

# Returns the equivalent C++ type, which could be None
def cpp_func_sig_arg_type_for(ctype):
    # The type is almost the same as for the function body, with the 
    # exception that pointers are converted to references
    is_reference = ctype[-1] == "*"

    ctype = cpp_func_body_arg_type_for(ctype)
    if ctype and is_reference:
        ctype += "&"

    return ctype

# This class is the bridge between the Function class which holds the code
# and the sub-classes which specialise behaviour for class methods and static procs
# Behaviour that is common across all function types is defined here...
class FunctionDelegate:
    def __init__(self, owner_class, function):
        self._function = function
        self._owner_class = owner_class

        # maps carg to cpparg
        self._arg_map = self.transform_args()

    def __getattr__(self, name):
        return getattr(self._function, name)

    # We'll assume int means GribStatus
    @property
    def return_type(self):
        if self._return_type == "int":
            debug_line("return_type", f"Setting {self._name} return type to GribStatus")
            return "GribStatus"
        else:
            return super().return_type()

    @property
    def cpp_args(self):
        return ", ".join([f"{a.type} {a.name}" for a in self.transformed_args])

    # This is called from __init__ to transform the args from C to C++ and store in the arg_map
    # Override as required...
    def transform_args(self):
        arg_map = {}
        for arg in self._cargs:
            cpp_type = cpp_func_sig_arg_type_for(arg.type)
            cpp_name = transform_variable_name(arg.name)
            if cpp_type:
                arg_map[arg] = Arg(cpp_name, cpp_type)
            else:
                arg_map[arg] = None

        return arg_map

    @property
    def transformed_name(self):
        return transform_function_name(self._name)

    @property
    def transformed_args(self):
        args = []
        for arg in self._cargs:
            if arg in self._arg_map:
                cpp_arg = self._arg_map[arg]
                if cpp_arg:
                    args.append(cpp_arg)

        return args

    def skip_line(self, line):
        if not line:
            debug_line("skip_line", f"[Empty]: {line}")
            return True

        if re.match(r"^\s*/[/\*]?", line):
            debug_line("skip_line", f"[Comment]: {line}")
            return True
        
        return False

    # Override this to provide any initial conversions before the main update_line runs
    def update_line_initial_pass(self, line):
        return line

    def process_return_variables(self, line):
        # Update any well know return values
        ret = "GribStatus"
        if self.return_type == ret:
            line_b4 = line
            line,count = re.subn(r"\bint\b(\s+err\s+=\s*)(\d+)[,;]", rf"{ret}\1{ret}{{\2}};", line)
            if count:
                debug_line("process_return_variables", f"return values [before]: {line_b4}")
                debug_line("process_return_variables", f"return values [after ]: {line}")
            line_b4 = line
            line,count = re.subn(r"(\(\s*err\s*)\)", rf"\1 != {ret}::SUCCESS)", line)
            if count:
                debug_line("process_return_variables", f"return values [before]: {line_b4}")
                debug_line("process_return_variables", f"return values [after ]: {line}")

        return line
                
    # Find variable declarations with assignments, e.g. char* buf = "Data"; and then:
    # 1. Update the type if required / delete the line if no longer valid
    # 2. Store in the arg_map for future reference
    def process_variable_declarations(self, line):
        line = self.process_return_variables(line)

        # Note: "return x;" looks like a variable declaration, so we explicitly exclude this...
        m = re.match(r"^\s*(?!return)(const)?\s*([\w\d]+\**)\s+([\w\d]+)\s*[=;]", line)

        if m:
            carg = Arg(m.group(3), m.group(2))
            cpp_type = cpp_func_body_arg_type_for(carg.type)
            if not cpp_type:
                debug_line("process_variable_declarations", f"Found var declaration to delete: {carg.type} {carg.name}")
                self._arg_map[carg] = None
                debug_line("process_variable_declarations", f"--> deleting: {line}")
                return "" #None
            else:
                cpp_arg = Arg(transform_variable_name(carg.name), cpp_type)
                debug_line("process_variable_declarations", f"Found var declaration to store: {carg.type} {carg.name} -> {cpp_arg.type} {cpp_arg.name}")
                self._arg_map[carg] = cpp_arg
                if carg.type != cpp_arg.type or carg.name != cpp_arg.name:
                    line = re.sub(rf"{re.escape(carg.type)}\s*{re.escape(carg.name)}", f"{cpp_arg.type} {cpp_arg.name}", line)
                    debug_line("process_variable_declarations", f"Transformed line: {line}")

        return line

    # Find variable declarations for char[] types and convert to std::string
    def process_string_declarations(self, line):
        m = re.match(r"^\s*([\w\d]+\**)\s+([\w\d]+)\s*(\[\d*\])\s*=", line)
        if m:
            carg = Arg(m.group(2), f"{m.group(1)}{m.group(3)}")
            cpp_arg = Arg(transform_variable_name(carg.name), "std::string")
            debug_line("process_string_declarations", f"Found string declaration to transform - adding to dict: {carg.type} {carg.name}")
            self._arg_map[carg] = cpp_arg
            debug_line("process_string_declarations", f"--> transforming to: {cpp_arg.type} {cpp_arg.name} [before]: {line}")
            line = re.sub(r"([\w\d]+\**)\s+([\w\d]+)\s*(\[\d*\])\s*=", f"{cpp_arg.type} {cpp_arg.name} =", line)
            debug_line("process_string_declarations", f"--> transforming to: {cpp_arg.type} {cpp_arg.name} [after ]: {line}")
        return line
    
    # Remove any variables that have been marked for deletion
    def process_deleted_variables(self, line):
        # Find any deleted variables that are being assigned to, and delete the line
        m = re.match(r"^\s*\**([\w\d]+)\s*=", line)
        if m:
            debug_line("process_deleted_variables", f"Found var assignment: var={m.group(1)}: {line}")
            for carg, cpp_arg in self._arg_map.items():
                if carg.name == m.group(1) and not cpp_arg:
                    debug_line("process_deleted_variables", f"Var assignment marked for delete, var={m.group(1)} - deleting: {line}")
                    line = f"/* {m.group(1)} removed */ // " + line
                    return line
        
        # Remove any deleted vars that remain (i.e. as an argument to a function call)
        for carg, cpp_arg in self._arg_map.items():
            m = re.match(rf"^.*\b{carg.name}\b\s*,*", line)
            if m and not cpp_arg:
                line_b4 = line
                line = re.sub(rf"[&\*]?\b{carg.name}(->)?\b\s*,*", "", line)
                debug_line("process_deleted_variables", f"Removing arg={carg.name} [before]: {line_b4}")
                debug_line("process_deleted_variables", f"Removing arg={carg.name} [after ]: {line}")

        return line

    def apply_variable_transforms(self, line):
        # Assume sizeof(x)/sizeof(*x) is now a container with a size() member...
        line_b4 = line
        line, count = re.subn(r"\b(.*=\s+)sizeof\((.*)\)\s*/\s*sizeof\(\*\2\);", r"\1\2.size();", line)
        if count:
            debug_line("apply_variable_transforms", f"size transform [before]: {line_b4}")
            debug_line("apply_variable_transforms", f"size transform [after ]: {line}")

        return line

    # Special handling for function pointers: [typedef] RET (*PFUNC)(ARG1, ARG2, ..., ARGN);
    def process_function_pointers(self, line):

        m = re.match(r"^(?:typedef)\s*([\w\d]+\**)\s*\(\s*\*(\s*[\w\d_]+)\s*\)\s*\((.*)\)", line)
        if m:
            carg_type = m.group(1)
            carg_name = m.group(2)
            cpp_var_type = cpp_func_body_arg_type_for(carg_type)
            cpp_var_name = transform_variable_name(carg_name)

            # Assume functions returning int will now return GribStatus
            if cpp_var_type == "int":
                cpp_var_type = "GribStatus"

            debug_line("process_function_pointers", f"Adding var to arg map: {carg_type} {carg_name} -> {cpp_var_type} {cpp_var_name} [before]: {line}")
            self._arg_map[Arg(carg_name,carg_type)] = Arg(cpp_var_name,cpp_var_type)

            # Parse the function arg types
            cpp_arg_types = []
            for arg_type in [a.strip() for a in m.group(3).split(",")]:
                cpp_arg_type = cpp_func_sig_arg_type_for(arg_type)
                if cpp_arg_type:
                    cpp_arg_types.append(cpp_arg_type)
            
            # Apply the transform
            line = re.sub(rf"([\w\d]+\**)\s*\(\s*\*(\s*[\w\d_]+)\s*\)\s*\((.*)\)", f"{cpp_var_type}(*{cpp_var_name})({','.join([a for a in cpp_arg_types])})", line)
            debug_line("process_function_pointers", f"Transformed line: {line}")

        return line
    
    def process_remaining_cargs(self, line):
        # Update any C Args that remain (i.e. as an argument to a function call)
        # This will also remove any unnecessary pointers/refs
        # Note: We ignore anything in quotes!
        for carg, cpp_arg in self._arg_map.items():
            if not cpp_arg:
                continue

            # Note: We assume *var and &var should be replaced with var
            m = re.search(rf"(?<!\")[&\*]?\b{carg.name}\b(\s*,*)(?!\")", line)

            if m and m.group(0) != cpp_arg.name:
                line = re.sub(m.re, rf"{cpp_arg.name}{m.group(1)}", line)
                debug_line("process_remaining_cargs", f"Substituted \"{m.group(0)}\" with \"{cpp_arg.name}{m.group(1)}\" [after ]: {line}")

        return line
            
    def convert_grib_values(self, line):
        for k, v in convert_data.GribStatusConverter.items():
            line, count = re.subn(rf"{k}", rf"{v}", line)
            if count:
                debug_line("convert_grib_values", f"Replaced {k} with {v} [after ]: {line}")
        
        for k, v in convert_data.GribTypeConverter.items():
            line, count = re.subn(rf"{k}", rf"{v}", line)
            if count:
                debug_line("convert_grib_values", f"Replaced {k} with {v} [after ]: {line}")
        
        return line
        
    def convert_grib_utils(self, line):
        for util in convert_data.GribUtilFuncs:
            m = re.search(rf"\b{util}\b", line)
            if m:
                cpp_util = transform_function_name(util)
                line = re.sub(m.re, f"{cpp_util}", line)
                debug_line("convert_grib_utils", f"{self._name} replaced {util} with {cpp_util} [after ]: {line}")

        return line
    
    def apply_function_substitutions(self, line):
        for k, v in basic_function_substitutions.items():
            line_b4 = line
            line, count = re.subn(k, v, line)
            if count:
                debug_line("apply_function_substitutions", f"{self._name} [before]: {line_b4}")
                debug_line("apply_function_substitutions", f"{self._name} [after ]: {line}")

        # Static function substitutions
        for f in self._owner_class._static_functions:
            m = re.search(rf"(?<!\")(&)?\b{f.name}\b(?!\")", line)
            if m:
                prefix = m.group(1) if m.group(1) is not None else ""
                line = re.sub(m.re, rf"{prefix}{transform_function_name(f.name)}", line)
                debug_line("apply_function_substitutions", f"Updating static function {m.group(0)} [after ]: {line}")

        return line

    # Override this to provide any final conversions after the main update_line runs
    def update_line_final_pass(self, line):
        return line

    def update_line(self, line):
        if self.skip_line(line):
            return line

        # Note: These apply in order, be careful if re-arranging!
        line = self.update_line_initial_pass(line)
        line = self.process_variable_declarations(line)
        line = self.process_string_declarations(line)
        line = self.process_deleted_variables(line)
        # process_deleted_variables may delete/comment the line...
        if self.skip_line(line):
            return line
        line = self.apply_variable_transforms(line)
        line = self.process_function_pointers(line)
        line = self.process_remaining_cargs(line)
        line = self.convert_grib_values(line)
        line = self.convert_grib_utils(line)
        line = self.apply_function_substitutions(line)
        line = self.update_line_final_pass(line)

        return line

    # This is the main entry point for updating the function body...        
    # It can be called recursively to update split lines etc
    def update_body(self, lines):
        new_lines = []

        for line in lines:
            if not line:
                # Assume this is a blank line in the input, so retain it for clarity...
                new_lines.append(line)
                continue
            
            # Split comma-separated variable definitions into separate lines
            # We then call ourself recursively for each split line
            m = re.match(r"^(\s*[\w\d]+\s+)[\w\d]+\s*=\s*[\w\d]+,", line)
            if m:
                debug_line("update_body", f"comma-separated vars [before]: {line}")
                line = line.replace(",", f";\n{m.group(1)}")
                split_lines = [l for l in line.split("\n")]
                for line in split_lines:
                    debug_line("update_body", f"comma-separated vars [after ]: {line}")
                split_lines = self.update_body(split_lines)
                new_lines.extend(split_lines)
                continue
            
            line = self.update_line(line)
            if line:
                new_lines.append(line)

        return new_lines
    
    @property
    def body(self):
        debug_line("body", f"-------------------- {self._name} [IN] --------------------")
        lines = self.update_body(self.code)
        debug_line("body", f"-------------------- {self._name} [OUT] -------------------")
        return "\n".join(lines)
        

# Definition of a class method
class Method(FunctionDelegate):
    def __init__(self, owner_class, function, const):
        super().__init__(owner_class, function)
        self._const = const
        self._owner_arg_type = self._owner_class.name
        self._owner_arg_name = transform_variable_name(self._owner_arg_type)

    @property
    def const(self):
        return "const" if self._const else ""

    # Override this to provide any initial conversions before the main update_line runs
    def update_line_initial_pass(self, line):
        line = self._owner_class.update_class_members(line)
        return line


# Represent a function signature
class FuncSig:
    def __init__(self, ret, name, args) -> None:
        self.ret = ret
        self.name = name
        self.args = args

# Override to add the "this" object to the method args
class InheritedMethod(Method):
    def __init__(self, owner_class, function, const):
        super().__init__(owner_class, function, const)
    
    # The following static dictionary defines well-known conversions from C to the equivalent
    # AccessorData virtual functions
    # [Key]     Name of an inherited function as defined in the C code
    # [Value]   Signature for the equivalent function in C++
    #           The argument list provides a 1:1 mapping from the C equivalent function - this map is also used 
    #           when replacing the use of these in the function body
    #           The C signature is shown in the comments to make it easier to identify which C arg maps to C++
    #           Note: some C args don't have a C++ equivalent, so are listed as None to maintain correct mapping
    func_sig_conversion = {
        # static int pack_TYPE(grib_accessor* a, const TYPE* v, size_t* len)
        "pack_string"         : FuncSig("GribStatus", "pack",   [None, Arg("value",  "std::string const&"),              None]),
        "pack_long"           : FuncSig("GribStatus", "pack",   [None, Arg("values", "std::vector<long> const&"),        None]),
        "pack_double"         : FuncSig("GribStatus", "pack",   [None, Arg("values", "std::vector<double> const&"),      None]),
        "pack_float"          : FuncSig("GribStatus", "pack",   [None, Arg("values", "std::vector<float> const&"),       None]),
        "pack_string_array"   : FuncSig("GribStatus", "pack",   [None, Arg("values", "std::vector<StringArray> const&"), None]),
        "pack_bytes"          : FuncSig("GribStatus", "pack",   [None, Arg("values", "std::vector<std::byte> const&"),   None]),

        # static int unpack_TYPE(grib_accessor* a, TYPE* v, size_t* len)
        "unpack_string"       : FuncSig("GribStatus", "unpack", [None, Arg("value",  "std::string&"),                    None]),
        "unpack_long"         : FuncSig("GribStatus", "unpack", [None, Arg("values", "std::vector<long>&"),              None]),
        "unpack_double"       : FuncSig("GribStatus", "unpack", [None, Arg("values", "std::vector<double>&"),            None]),
        "unpack_float"        : FuncSig("GribStatus", "unpack", [None, Arg("values", "std::vector<float>&"),             None]),
        "unpack_string_array" : FuncSig("GribStatus", "unpack", [None, Arg("values", "std::vector<StringArray>&"),       None]),
        "unpack_bytes"        : FuncSig("GribStatus", "unpack", [None, Arg("values", "std::vector<std::byte>&"),         None]),

        # static int unpack_TYPE_element(grib_accessor*, size_t i, TYPE* val);
        "unpack_double_element"     : FuncSig("GribStatus", "unpackElement",    [None, Arg("index", "std::size_t"), Arg("val","double&")]),
        "unpack_float_element"      : FuncSig("GribStatus", "unpackElement",    [None, Arg("index", "std::size_t"), Arg("val","float&")]),
        
        # static int unpack_TYPE_element_set(grib_accessor*, const size_t* index_array, size_t len, TYPE* val_array);
        "unpack_double_element_set" : FuncSig("GribStatus", "unpackElementSet", [None, Arg("indexArray", "std::vector<std::size_t> const&"), None, Arg("valArray","std::vector<double>&")]),
        "unpack_float_element_set"  : FuncSig("GribStatus", "unpackElementSet", [None, Arg("indexArray", "std::vector<std::size_t> const&"), None, Arg("valArray","std::vector<float>&")]),
        
        # static int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len);
        "unpack_double_subarray"    : FuncSig("GribStatus", "unpackSubArray",   [None, Arg("values", "std::vector<double>&"), None, Arg("start","std::size_t")]),
        
        # Double arg functions of the form: RET func_name(grib_accessor*, TYPE);
        # static int pack_expression(grib_accessor*, grib_expression*);
        "pack_expression" : FuncSig("GribStatus", "pack",           [None, Arg("expression", "grib_expression const&")]),
        # static int value_count(grib_accessor*, long*);
        "value_count"     : FuncSig("long", "valueCount",           [None, None]),
        # static void update_size(grib_accessor*, size_t);
        "update_size"     : FuncSig("void", "updateSize",           [None, Arg("s", "std::size_t")]),
        # static size_t preferred_size(grib_accessor*, int);
        "preferred_size"  : FuncSig("std::size_t", "preferredSize", [None, Arg("fromHandle", "int")]),
        # static int compare(grib_accessor*, grib_accessor*);
        "compare"         : FuncSig("bool", "compare",              [None, Arg("rhs", "AccessorData const&")]),
        # static void resize(grib_accessor*,size_t);
        "resize"          : FuncSig("void", "resize",               [None, Arg("newSize", "std::size_t")]),

        # Single arg functions of the form: RET func_name(grib_accessor*);
        # static int get_native_type(grib_accessor*);
        "get_native_type" : FuncSig("GribType", "nativeType",       [None]),
        # static int pack_missing(grib_accessor*);
        "pack_missing"    : FuncSig("GribStatus", "packMissing",    [None]),
        # static size_t string_length(grib_accessor*);
        "string_length"   : FuncSig("std::size_t", "stringLength",  [None]),
        # static long byte_count(grib_accessor*);
        "byte_count"      : FuncSig("long", "byteCount",            [None]),
        # static long byte_offset(grib_accessor*);
        "byte_offset"     : FuncSig("long", "byteOffset",           [None]),
    }
    
    @property
    def return_type(self):
        if self._name in self.func_sig_conversion:
            return self.func_sig_conversion[self._name].ret
        else:
            return super().return_type()
    
    @property
    def transformed_name(self):
        if self._name in self.func_sig_conversion:
            return self.func_sig_conversion[self._name].name
        else:
            return super().transformed_name()
    
    def transform_args(self):
        if self._name in self.func_sig_conversion:
            arg_map = {}
            for index, arg in enumerate(self._cargs):
                cpp_arg = self.func_sig_conversion[self._name].args[index]
                if cpp_arg:
                    arg_map[arg] = cpp_arg
                else:
                    arg_map[arg] = None

            return arg_map
        
        return super().transform_args()

class PrivateMethod(Method):
    pass

class DestructorMethod(Method):
    pass

class ConstructorMethod(Method):

    # From: init(grib_accessor* a, const long len, grib_arguments* arg)
    # To:   CONSTRUCTOR(AccessorInitData const& initData)
    def transform_args(self):
        arg_map = {}
        for arg in self._cargs:
            if arg.type == "grib_arguments*":
                arg_map[arg] = Arg("initData", "AccessorInitData const&")
            else:
                arg_map[arg] = None

        return arg_map
        
    @property
    def call_args(self):
        return ", ".join([a.name for a in self.transformed_args if a])

    def update_line_initial_pass(self, line):
        
        line = re.sub(rf"\ba->(\w+)\b", rf"/* TO DO */ //\1", line)
        line = re.sub(r"\ba\b", f"*this", line)

        # Now transform the argument getters
        line = re.sub(rf"\bgrib_arguments_get_name\(h, arg, (\d+)\)", rf"AccessorName(std::get<std::string>(initData[\1].second))", line)
        line = re.sub(rf"\bgrib_arguments_get_(\w+)\(h, arg, (\d+)\)", rf"std::get<\1>(initData[\2].second)", line)

        return super().update_line_initial_pass(line)


class CompareMethod(Method):
    pass
    '''
    def transform_args(self):
        pass
        #return "const Accessor* other"

    def update_body(self, lines):
        args = self.args
        assert len(args) == 2
        assert args[0].type == "grib_accessor*"
        assert args[1].type == "grib_accessor*"

        a = args[0].name
        b = args[1].name

        result = []
        for line in lines:
            line = re.sub(rf"\b{a}\b", "this", line)
            line = re.sub(rf"\b{b}\b", "other", line)
            result.append(line)

        return result
    '''

class DumpMethod(Method):
    @property
    def body(self):
        return "\n".join(
            ["#if 0"]
            + self.code
            + ["#endif", "throw AccessorException(GRIB_NOT_IMPLEMENTED);"]
        )

class StaticProc(FunctionDelegate):
    def __init__(self, owner_class, function):
        super().__init__(owner_class, function)

    def update_line_final_pass(self, line):
        line = self._owner_class.update_static_function_calls(line)
        return line

class GlobalFunction(FunctionDelegate):
    def __init__(self, owner_class, function):
        super().__init__(owner_class, function)

    # Overridden as there's no opening and closing { } so don't want to assert!
    @property
    def code(self):
        return self._lines

SPECIALISED_METHODS = {
    ("accessor", "compare"): CompareMethod,
    ("accessor", "dump"): DumpMethod,
}

class Class:
    rename = {}

    def __init__(
        self,
        *,
        path,
        class_,
        functions,
        includes,
        factory_name,
        super,
        implements,
        members,
    ):
        assert factory_name is not None

        self._class = class_
        # EG: name=ProjStringData, cname=grib_accessor_class_proj_string
        self._name, self._cname = self.tidy_class_name(path)

        self._factory_name = factory_name
        self._members = members
        self._functions = functions
        self._implements = implements
        self._body_includes = includes
        self._forward_declarations = []

        if super is None:
            self._super, _ = self.tidy_class_name(class_)
            self._include_dir = "cpp"
            self._top = True
        else:
            self._super, _ = self.tidy_class_name(super)

            if self._super == "AccessorData":
                self._include_dir = "cpp"
                self._top = True
            else:
                self._include_dir = ARGS.target
                self._top = False

        # Load the patch if it exists
        try:
            m = importlib.import_module(f"conversion/patches.{self._cname}")
            if "func_sig_conversion_patch" in dir(m):
                self._func_sig_conversion_patch = m.func_sig_conversion_patch
        except ModuleNotFoundError:
            self._func_sig_conversion_patch = None

    def finalise(self, other_classes):
        self._other_classes = other_classes

        if not self._top and self._super not in self._other_classes:
            raise Exception(f"Unknown super class {self._super}")

        for m in self._members:
            if m.name in self.top_members:
                LOG.warning(f'{self._cname}.cc member "{m.name}" is already defined')
            if m.type == self.type_name + "*":
                m.type = self.class_name + "*"
                # Warn as this is probably NOT what we want!
                debug_line("finalise", f"name={self._name} : ***WARNING*** member {m.name}'s type converted from {self.type_name}* to {m.type}")

        # Classify functions
        self._inherited_methods = []
        self._private_methods = []
        self._static_functions = []

        # Inherited functions
        for name, f in list(self._functions.items()):
            if name in self._implements and name not in ("init", "destroy"):
                METHOD = SPECIALISED_METHODS.get((self._class, name), InheritedMethod)
                self._inherited_methods.append(
                    METHOD(self, f, const=name not in self.non_const_methods)
                )
                del self._functions[name]

        # Constructor
        if "init" in self._functions:
            self._constructor = ConstructorMethod(
                self,
                self._functions["init"],
                const=False,
            )
            del self._functions["init"]
        else:
            self._constructor = ConstructorMethod(
                self,
                Function("init", "void", self.constructor_args),
                const=False,
            )

        # Destructor
        if "destroy" in self._functions:
            self._destructor = DestructorMethod(
                self,
                self._functions["destroy"],
                const=False,
            )
            del self._functions["destroy"]
        else:
            self._destructor = DestructorMethod(
                self,
                Function("destroy", "void", ""),
                const=False,
            )

        # Other functions
        ptr_type_name = self.type_name + "*" # grib_accessor*
        for name, f in list(self._functions.items()):
            if f._return_type == ptr_type_name:
                f._return_type = self.class_name + "*"

            if name == global_function_name:
                self._global_function = GlobalFunction(self, f)
                del self._functions[name]
                continue

            # If first arg starts with ptr_type_name, then it's a private method (as we've already extracted inherited functions)
            if f.args[0].type == ptr_type_name:
                self._private_methods.append(
                    PrivateMethod(self, f, const=name not in self.non_const_methods)
                )
                del self._functions[name]
            else:
                self._static_functions.append(StaticProc(self, f))
                del self._functions[name]

        assert len(self._functions) == 0, sorted(self._functions.keys())
        self.apply_patches()

    def apply_patches(self):
        try:
            m = importlib.import_module(f"conversion/patches.{self._cname}")
        except ModuleNotFoundError:
            return
        LOG.info(f"Applying patches for {self._cname}")
        m.patch(self)

    def cannot_convert_method(self, name):
        ok = False
        for m in self._inherited_methods:
            if m.name == name:
                m.cannot_convert()
                ok = True
        for m in self._private_methods:
            if m.name == name:
                m.cannot_convert()
                ok = True
        for m in self._static_functions:
            if m.name == name:
                m.cannot_convert()
                ok = True

        if name == 'init':
            self._constructor.cannot_convert()
            ok = True

        if name == 'destroy':
            self._destructor.cannot_convert()
            ok = True

        if not ok:
            raise Exception(f"Cannot convert method {name}")

    def mark_mutable(self, name):
        ok = False
        for m in self._members:
            if m.name == name:
                m._mutable = True
                ok = True

        if not ok:
            raise Exception(f"Cannot convert member {name}")

    def add_forward_declaration(self, name):
        self._forward_declarations.append(name)

    @property
    def name(self):
        return self._name
    
    @property
    def name_camel_case(self):
        return transform_variable_name(self._name)

    @property
    def cname(self):
        return self._cname

    @property
    def factory_name(self):
        return self._factory_name

    @property
    def super(self):
        return self._super

    @property
    def members(self):
        return self._members

    # Get members all the way up the hierarchy
    @property
    def members_in_hierarchy(self):
        result = set(self.top_members)
        for m in self._members:
            result.add(m)
            #result.add(m.name)
        if self.super in self._other_classes:
            result.update(self._other_classes[self.super].members_in_hierarchy)
        return result
        #return sorted(result)

    @property
    def include_super(self):
        if self._super == "AccessorData":
            return "/".join(["AccessorData", self._super + ".h"])
        else:
            return "/".join([ARGS.target] + self.namespaces + [self._super + ".h"])
        # return "/".join([self._include_dir] + self.namespaces + [self._super + ".h"])

    @property
    def include_header(self):
        return "/".join([ARGS.target] + self.namespaces + [self._name + ".h"])

    @property
    def header_includes(self):
        return []

    @property 
    def common_includes(self):
        return common_includes
        # return ["/".join([self._include_dir] + self.namespaces + [inc]) for inc in common_includes]

    @property
    def body_includes(self):
        return self._body_includes

    @property
    def namespaces(self):
        return ["eccodes", self._class]

    @property
    def nested_namespaces(self):
        return "::".join(["eccodes", self._class])
    
    @property
    def namespaces_reversed(self):
        return reversed(self.namespaces)

    @property
    def global_function(self):
        return self._global_function

    @property
    def constructor(self):
        return self._constructor

    @property
    def destructor(self):
        return self._destructor

    @property
    def inherited_methods(self):
        return self._inherited_methods

    @property
    def private_methods(self):
        return self._private_methods

    @property
    def static_functions(self):
        return self._static_functions

    @property
    def forward_declarations(self):
        return self._forward_declarations

    def dump(self):
        self.dump_header()
        self.dump_body()

    def tidy_class_name(self, path):
        path, ext = os.path.splitext(path)
        c_name = os.path.basename(path)

        name = c_name.replace(self.prefix, "")

        name = transform_class_name(name)

        return self.rename.get(name, name) + "Data", c_name

    def save(self, ext, content):
        target = os.path.join(ARGS.target, *self.namespaces, f"{self.name}.{ext}")
        LOG.info("Writing %s", target)

        tmp = os.path.join(ARGS.target, *self.namespaces, f"{self.name}-tmp.{ext}")
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

    def dump_header(self):
        template = env.get_template(f"{self._class}Data.h.j2")
        self.save("h", template.render(c=self))

    def dump_body(self):
        # Beware of this: https://github.com/pallets/jinja/issues/604
        template = env.get_template(f"{self._class}Data.cc.j2")

        def tidy_more(text):
            # Some more tidying

            text = re.sub(
                r"/\*\s+This is used by make_class.pl\s+\*/",
                "",
                text,
                re.MULTILINE,
            )

            return text

        self.save("cc", tidy_more(template.render(c=self)))

    def update_class_members(self, line):
        line_b4 = line
        line,count = re.subn(r"\bsuper->\b", f"{self.super}::", line)
        if count:
            debug_line("update_class_members", f"begin [before]: {line_b4}")
            debug_line("update_class_members", f"begin [after ]: {line}")

        accessor_variable_name = transform_variable_name(self._name)

        for n in [r"\bself\b"]:
            line_b4 = line
            line,count = re.subn(n, f"{accessor_variable_name}", line)
            if count:
                debug_line("update_class_members", f"this [before]: {line_b4}")
                debug_line("update_class_members", f"this [after ]: {line}")

        if re.match(rf".*\b{accessor_variable_name}\s+=", line):
            debug_line("update_class_members", f"deleting: {line}")
            line = ""

        if re.match(rf".*\bsuper\s+=\s+\*\({accessor_variable_name}->cclass->super\)", line):
            debug_line("update_class_members", f"deleting: {line}")
            line = ""

        for m in self.members_in_hierarchy:
            line_b4 = line
            line,count = re.subn(rf"\b{accessor_variable_name}->{m.cname}\b", rf"{m.name}", line)
            if(count):
                debug_line("update_class_members", f"members_in_hierarchy [before]: {line_b4}")
                debug_line("update_class_members", f"members_in_hierarchy [after ]: {line}")

        for m in self._private_methods:
            name = m.name
            line_b4 = line
            line,count = re.subn(rf"\b{name}\s*\(\s*([^,]+),", f"{accessor_variable_name}.{name}(", line)
            if(count):
                debug_line("update_class_members", f"_private_methods [before]: {line_b4}")
                debug_line("update_class_members", f"_private_methods [after ]: {line}")

        return line

    def update_static_function_calls(self, line):
        for m in self._static_functions:
            name = m.name
            line_b4 = line
            line,count = re.subn(rf"\b{name}\s*\(", f"{transform_function_name(name)}(", line)
            if(count):
                debug_line("update_static_function_calls", f"name={name} [before]: {line_b4}")
                debug_line("update_static_function_calls", f"name={name} [after ]: {line}")

        return line

    def tidy_top_level(self, line):
        for m in self._static_functions:
            name = m.name
            line,count = re.subn(rf"\b{name}\b", f"{transform_function_name(name)}", line)

        return line

class Accessor(Class):
    class_name = "Accessor"
    type_name = "grib_accessor"
    constructor_args = "AccessorInitData const&"

    top_members = [
        # "name_",
        # "length_",
        # "offset_",
        # "dirty_",
        # "flags_",
        # "context_",
        # "parent_",
        # "next_",
        # "h_",
    ]

    non_const_methods = [
        "pack_long",
        "pack_missing",
        "pack_string",
        "pack_double",
        "pack_bytes",
        "pack_expression",
        "pack_string_array",
        "update_size","notify_change",
    ]

    # namespaces = ["eccodes", "accessor"]
    prefix = "grib_accessor_class_"
    rename = {
        "Gen": "Accessor", # "Generic",
        "Md5": "Md5Sum",  # We rename because of macos case insensitive file system
        "Assert": "Assertion", # Name clash with assert.h
    }

    def class_to_type(self):
        return self.cname.replace("_class_", "_")

def parse_file(path):
    in_definition = False
    in_implementation = False
    in_function = False
    includes = []
    factory_name = None
    template = None
    depth = 0

    definitions = {}
    functions = {}

    # Create a global function for storing global vars etc
    global_function = functions[global_function_name] = Function(
        global_function_name,
        None,
        [],
        template,
    )

    LOG.info("Parsing %s", path)

    f = open(path, "r")
    for line in f:
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
            continue

        if stripped_line.startswith("/* END_CLASS_IMP */"):
            in_implementation = False
            continue

        if in_implementation:
            m = re.match(r"\s*\"(\w+)\",\s+/\* name \*/", stripped_line)
            if m:
                factory_name = m.group(1)
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

        m = re.match(r"static\s+([^(]+)\s+(\w+)\s*\(([^(]+)\)", line)
        if m:
            if line.rstrip().endswith(");"):
                # Forward declaration
                continue

            assert not in_function, line
            function_name = m.group(2)

            in_function = True
            function = functions[function_name] = Function(
                function_name,
                m.group(1),
                m.group(3),
                template,
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

        if stripped_line.startswith("#include"):
            if len(includes) == 0:
                # Forget lines before the first include
                global_function.clear_lines()
            if 'minmax_val' not in line:
                includes.append(line[9:])
            continue

        if stripped_line.startswith("template "):
            template = stripped_line
            continue

        global_function.add_line(line)

    if definitions:
        class_ = definitions.pop("CLASS")[0]
        accessorImpl = Accessor(
            path=path,
            class_=class_,
            functions=functions,
            includes=includes,
            factory_name=factory_name,
            super=definitions["SUPER"][0] if "SUPER" in definitions else None,
            implements=definitions.get("IMPLEMENTS", []),
            members=[Member(m) for m in definitions.get("MEMBERS", []) if m != ""],
        )
        return accessorImpl


def write_makefile(class_list):
    template = env.get_template("CMakeLists.txt.j2")
    content = template.render(c=class_list)

    target = os.path.join(ARGS.target, "CMakeLists.txt")
    LOG.info("Writing %s", target)

    os.makedirs(os.path.dirname(target), exist_ok=True)
    with open(target, "w") as f:
        f.write(content)

ignore_files = ["grib_accessor_class_gen.cc"]

def main():
    classes = {}

    for a in ARGS.path:
        if a in ignore_files:
            LOG.info("Ignoring file %s", a)
        else:
            accessorImpl = parse_file(a)
            if accessorImpl is not None:
                classes[accessorImpl.name] = accessorImpl

    LOG.info("Finalising %s classes", len(classes))
    for accessorImpl in classes.values():
        accessorImpl.finalise(classes)

    class_list = []

    for accessorImpl in classes.values():
        class_list.append(accessorImpl.name)
        accessorImpl.dump()

    write_makefile(class_list)

if __name__ == "__main__":
    main()
