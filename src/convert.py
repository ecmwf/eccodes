#!/usr/bin/env python3

import argparse
import importlib
import os
import re
from collections import defaultdict
import logging

from jinja2 import Environment, FileSystemLoader, StrictUndefined

LOG = logging.getLogger(__name__)

parser = argparse.ArgumentParser()
parser.add_argument("--templates", default="j2")
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

def snake_to_pascal(s):
    return "".join(x.capitalize() for x in s.lower().split("_"))

def snake_to_camel(s):
    s = snake_to_pascal(s)
    return s[0].lower() + s[1:]

def pascal_to_camel(s):
    return s[0].lower() + s[1:]

def transform_function_name(name):
    name = re.sub(rf"^get_", f"", name)
    name = re.sub(rf"^set_", f"", name)

    return name

common_includes = ["AccessorUtils/AccessorProxy.h", "AccessorFactory.h"]

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

        self.name = snake_to_camel(self.cname) + "_"
        print(f"[Member] name={self.name} cname={self.cname}")

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

        self._arg_mappings = {}

        for arg in [a.strip() for a in args.split(",")]:
            if not arg:
                continue
            bits = arg.split()
            type = " ".join(bits[:-1])
            name = bits[-1]
            carg = Arg(name, type)
            self._cargs.append(carg)

        print(f"[Function] {self._name}(" + ", ".join([f"{a.type} {a.name}" for a in self._cargs]) + ")" )

    def update_lines(self, lines):
        self._lines = lines

    def is_empty(self):
        return len(self._lines) == 0

    def add_line(self, line):
        self._lines.append(line)

    @property
    def name(self):
        return self._name

    @property
    def transformed_name(self):
        return snake_to_camel(transform_function_name(self._name))

    @property
    def transformed_args(self):
        args = []
        for arg in self._cargs:
            arg.name = snake_to_camel(arg.name)
            args.append(arg)

        return args

    @property
    def args_declaration(self):
        return ", ".join([f"{a.type} {a.name}" for a in self.transformed_args])

    @property
    def call_args(self):
        return ", ".join([a.name for a in self.transformed_args])

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

class FunctionDelegate:
    def __init__(self, function):
        self._function = function

    def __getattr__(self, name):
        return getattr(self._function, name)

GribStatusConverter = {
    "GRIB_SUCCESS": "GribStatus::SUCCESS", 
    "GRIB_END_OF_FILE": "GribStatus::END_OF_FILE", 
    "GRIB_INTERNAL_ERROR": "GribStatus::INTERNAL_ERROR", 
    "GRIB_BUFFER_TOO_SMALL": "GribStatus::BUFFER_TOO_SMALL", 
    "GRIB_NOT_IMPLEMENTED": "GribStatus::NOT_IMPLEMENTED", 
    "GRIB_7777_NOT_FOUND": "GribStatus::VALUE_7777_NOT_FOUND", 
    "GRIB_ARRAY_TOO_SMALL": "GribStatus::ARRAY_TOO_SMALL", 
    "GRIB_FILE_NOT_FOUND": "GribStatus::FILE_NOT_FOUND", 
    "GRIB_CODE_NOT_FOUND_IN_TABLE": "GribStatus::CODE_NOT_FOUND_IN_TABLE", 
    "GRIB_WRONG_ARRAY_SIZE": "GribStatus::WRONG_ARRAY_SIZE", 
    "GRIB_NOT_FOUND": "GribStatus::NOT_FOUND", 
    "GRIB_IO_PROBLEM": "GribStatus::IO_PROBLEM", 
    "GRIB_INVALID_MESSAGE": "GribStatus::INVALID_MESSAGE", 
    "GRIB_DECODING_ERROR": "GribStatus::DECODING_ERROR", 
    "GRIB_ENCODING_ERROR": "GribStatus::ENCODING_ERROR", 
    "GRIB_NO_MORE_IN_SET": "GribStatus::NO_MORE_IN_SET", 
    "GRIB_GEOCALCULUS_PROBLEM": "GribStatus::GEOCALCULUS_PROBLEM", 
    "GRIB_OUT_OF_MEMORY": "GribStatus::OUT_OF_MEMORY", 
    "GRIB_READ_ONLY": "GribStatus::READ_ONLY", 
    "GRIB_INVALID_ARGUMENT": "GribStatus::INVALID_ARGUMENT", 
    "GRIB_NULL_HANDLE": "GribStatus::NULL_HANDLE", 
    "GRIB_INVALID_SECTION_NUMBER": "GribStatus::INVALID_SECTION_NUMBER", 
    "GRIB_VALUE_CANNOT_BE_MISSING": "GribStatus::VALUE_CANNOT_BE_MISSING", 
    "GRIB_WRONG_LENGTH": "GribStatus::WRONG_LENGTH", 
    "GRIB_INVALID_TYPE": "GribStatus::INVALID_TYPE", 
    "GRIB_WRONG_STEP": "GribStatus::WRONG_STEP", 
    "GRIB_WRONG_STEP_UNIT": "GribStatus::WRONG_STEP_UNIT", 
    "GRIB_INVALID_FILE": "GribStatus::INVALID_FILE", 
    "GRIB_INVALID_GRIB": "GribStatus::INVALID_GRIB", 
    "GRIB_INVALID_INDEX": "GribStatus::INVALID_INDEX", 
    "GRIB_INVALID_ITERATOR": "GribStatus::INVALID_ITERATOR", 
    "GRIB_INVALID_KEYS_ITERATOR": "GribStatus::INVALID_KEYS_ITERATOR", 
    "GRIB_INVALID_NEAREST": "GribStatus::INVALID_NEAREST", 
    "GRIB_INVALID_ORDERBY": "GribStatus::INVALID_ORDERBY", 
    "GRIB_MISSING_KEY": "GribStatus::MISSING_KEY", 
    "GRIB_OUT_OF_AREA": "GribStatus::OUT_OF_AREA", 
    "GRIB_CONCEPT_NO_MATCH": "GribStatus::CONCEPT_NO_MATCH", 
    "GRIB_HASH_ARRAY_NO_MATCH": "GribStatus::HASH_ARRAY_NO_MATCH", 
    "GRIB_NO_DEFINITIONS": "GribStatus::NO_DEFINITIONS", 
    "GRIB_WRONG_TYPE": "GribStatus::WRONG_TYPE", 
    "GRIB_END": "GribStatus::END", 
    "GRIB_NO_VALUES": "GribStatus::NO_VALUES", 
    "GRIB_WRONG_GRID": "GribStatus::WRONG_GRID", 
    "GRIB_END_OF_INDEX": "GribStatus::END_OF_INDEX", 
    "GRIB_NULL_INDEX": "GribStatus::NULL_INDEX", 
    "GRIB_PREMATURE_END_OF_FILE": "GribStatus::PREMATURE_END_OF_FILE", 
    "GRIB_INTERNAL_ARRAY_TOO_SMALL": "GribStatus::INTERNAL_ARRAY_TOO_SMALL", 
    "GRIB_MESSAGE_TOO_LARGE": "GribStatus::MESSAGE_TOO_LARGE", 
    "GRIB_CONSTANT_FIELD": "GribStatus::CONSTANT_FIELD", 
    "GRIB_SWITCH_NO_MATCH": "GribStatus::SWITCH_NO_MATCH", 
    "GRIB_UNDERFLOW": "GribStatus::UNDERFLOW", 
    "GRIB_MESSAGE_MALFORMED": "GribStatus::MESSAGE_MALFORMED", 
    "GRIB_CORRUPTED_INDEX": "GribStatus::CORRUPTED_INDEX", 
    "GRIB_INVALID_BPV": "GribStatus::INVALID_BPV", 
    "GRIB_DIFFERENT_EDITION": "GribStatus::DIFFERENT_EDITION", 
    "GRIB_VALUE_DIFFERENT": "GribStatus::VALUE_DIFFERENT", 
    "GRIB_INVALID_KEY_VALUE": "GribStatus::INVALID_KEY_VALUE", 
    "GRIB_STRING_TOO_SMALL": "GribStatus::STRING_TOO_SMALL", 
    "GRIB_WRONG_CONVERSION": "GribStatus::WRONG_CONVERSION", 
    "GRIB_MISSING_BUFR_ENTRY": "GribStatus::MISSING_BUFR_ENTRY", 
    "GRIB_NULL_POINTER": "GribStatus::NULL_POINTER", 
    "GRIB_ATTRIBUTE_CLASH": "GribStatus::ATTRIBUTE_CLASH", 
    "GRIB_TOO_MANY_ATTRIBUTES": "GribStatus::TOO_MANY_ATTRIBUTES", 
    "GRIB_ATTRIBUTE_NOT_FOUND": "GribStatus::ATTRIBUTE_NOT_FOUND", 
    "GRIB_UNSUPPORTED_EDITION": "GribStatus::UNSUPPORTED_EDITION", 
    "GRIB_OUT_OF_RANGE": "GribStatus::OUT_OF_RANGE", 
    "GRIB_WRONG_BITMAP_SIZE": "GribStatus::WRONG_BITMAP_SIZE", 
    "GRIB_FUNCTIONALITY_NOT_ENABLED": "GribStatus::FUNCTIONALITY_NOT_ENABLED", 
    "GRIB_VALUE_MISMATCH": "GribStatus::VALUE_MISMATCH", 
    "GRIB_DOUBLE_VALUE_MISMATCH": "GribStatus::DOUBLE_VALUE_MISMATCH", 
    "GRIB_LONG_VALUE_MISMATCH": "GribStatus::LONG_VALUE_MISMATCH", 
    "GRIB_BYTE_VALUE_MISMATCH": "GribStatus::BYTE_VALUE_MISMATCH", 
    "GRIB_STRING_VALUE_MISMATCH": "GribStatus::STRING_VALUE_MISMATCH", 
    "GRIB_OFFSET_MISMATCH": "GribStatus::OFFSET_MISMATCH", 
    "GRIB_COUNT_MISMATCH": "GribStatus::COUNT_MISMATCH", 
    "GRIB_NAME_MISMATCH": "GribStatus::NAME_MISMATCH", 
    "GRIB_TYPE_MISMATCH": "GribStatus::TYPE_MISMATCH", 
    "GRIB_TYPE_AND_VALUE_MISMATCH": "GribStatus::TYPE_AND_VALUE_MISMATCH", 
    "GRIB_UNABLE_TO_COMPARE_ACCESSORS": "GribStatus::UNABLE_TO_COMPARE_ACCESSORS", 
    "GRIB_ASSERTION_FAILURE": "GribStatus::ASSERTION_FAILURE", 
}

class Method(FunctionDelegate):
    def __init__(self, owner_class, function, const):
        super().__init__(function)
        self._owner_class = owner_class
        self._const = const
        self._owner_arg_type = self._owner_class.name
        self._owner_arg_name = pascal_to_camel(self._owner_arg_type)
        self._args_to_delete = []


    @property
    def args_declaration(self):
        return ", ".join([f"{a.type} {a.name}" for a in self.transformed_args if a])

    @property
    def call_args(self):
        return ", ".join([a.name for a in self.transformed_args if a])

    @property
    def const(self):
        return "const" if self._const else ""

    # Whilst parsing, any variables with type defined here will be added to the
    # associated list, which is then used to delete later instances
    types_to_delete = {
        "grib_handle*"
    }

    def tidy_lines(self, lines):
        new_lines = []

        for line in lines:
            # Split comma-separated variable definitions into separate lines
            m = re.match(r"^(\s*[\w\d]+\s+)[\w\d]+\s*=\s*[\w\d]+,", line)
            if m:
                line_b4 = line
                line = line.replace(",", f";\n{m.group(1)}")
                print(f"[tidy_lines] comma-separated vars [before]: {line_b4} (*)---(*)---(*)")
                print(f"[tidy_lines] comma-separated vars [after ]: {line} (*)---(*)---(*)")

            # Look for variable assignments
            m = re.match(r"^\s*([\w\d]+\**)\s+([\w\d]+)\s*=", line)
            if m:
                # Find any types to delete
                for type in self.types_to_delete:
                    if type == m.group(1):
                        if m.group(2) not in self._args_to_delete:
                            print(f"[tidy_lines] Found var to delete - adding  : {m.group(1)} {m.group(2)}")
                            self._args_to_delete.append(m.group(2))
                            print(f"[tidy_lines] Found var to delete - deleting: {line}")
                            line = ""
                            #continue

            # Update (or remove if no longer used) any renamed args
            for index, carg in enumerate(self.args):
                arg = self.transformed_args[index]
                
                m = re.match(rf"^.*\b{carg.name}\b", line)
                if m:
                    line_b4 = line
                    if arg:
                        line = re.sub(rf"\b{carg.name}\b", f"{arg.name}", line)
                    else:
                        line = f"/* {carg.name} removed */ // " + line
                    print(f"[tidy_lines] transformed_args {self._name} [before]: {line_b4}")
                    print(f"[tidy_lines] transformed_args {self._name} [after ]: {line}")

            # Update any well know return values
            ret = "GribStatus"
            if self.return_type == ret:
                line_b4 = line
                line,count = re.subn(r"\bint\b(\s+err\s+=\s*)(\d+)[,;]", rf"{ret}\1{ret}{{\2}};", line)
                if count:
                    print(f"[tidy_lines] return values [before]: {line_b4}")
                    print(f"[tidy_lines] return values [after ]: {line}")
                line_b4 = line
                line,count = re.subn(r"(\(\s*err\s*)\)", rf"\1 != {ret}::SUCCESS)", line)
                if count:
                    print(f"[tidy_lines] return values [before]: {line_b4}")
                    print(f"[tidy_lines] return values [after ]: {line}")

            new_lines.append(line)

        return new_lines

    # Perform any final tidy up...
    def final_tidy_lines(self, lines):
        new_lines = []

        for line in lines:
            # Remove any remaining "vars_to_delete"
            for arg in self._args_to_delete:
                m = re.match(rf"^.*\b({arg}\s*,*\s*)", line)
                if m:
                    line_b4 = line
                    line = line.replace(f"{m.group(1)}", "")
                    print(f"[final_tidy_lines] deleted arg {arg} [before]: {line_b4}")
                    print(f"[final_tidy_lines] deleted arg {arg} [after ]: {line}")

            new_lines.append(line)

        return new_lines


    @property
    def body(self):
        this = [r"\bself\b"]

        # Don't need to add the first arg (grib_accessor* a) as this is
        # removed by the Inherited Method
        '''
        # Look for this-> or self-> or a->
        ptr_type_name = self._owner_class.type_name + "*"
        if len(self.args) > 0:
            arg = self.args[0]
            if arg.type == ptr_type_name:
                this.append(rf"\b{arg.name}\b")
        '''

        lines = self.tidy_lines(self.code)
        lines = [self._owner_class.tidy_line(n, this) for n in lines]
        lines = self.final_tidy_lines(lines)
        return "\n".join(lines)

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
        
    @property
    def transformed_args(self):
        if self._name in self.func_sig_conversion:
            return self.func_sig_conversion[self._name].args
        else:
            return super().transformed_args()


class PrivateMethod(Method):
    pass

class DestructorMethod(Method):
    pass

# Override to combine factory and constructor args
class ConstructorMethod(Method):

    @property
    def transformed_args(self):
        return [Arg("initData", "AccessorInitData const&")]

    def tidy_lines(self, lines):
        result = []
        for line in lines:
            m = re.match(rf"\bself->(\w+)\b", line)
            if m:
                name = snake_to_camel(m.group(1))
                line = re.sub(rf"\bself->(\w+)\b", rf"{name}_", line)

            line = re.sub(rf"\ba->(\w+)\b", rf"/* TO DO */ //\1", line)
            line = re.sub(r"\ba\b", f"*this", line)

            # Now transform the argument getters
            line = re.sub(rf"\bgrib_arguments_get_name\(h, arg, (\d+)\)", rf"AccessorName(std::get<std::string>(initData[\1].second))", line)
            line = re.sub(rf"\bgrib_arguments_get_(\w+)\(h, arg, (\d+)\)", rf"std::get<\1>(initData[\2].second)", line)

            result.append(line)

        return result

class CompareMethod(Method):
    @property
    def args_declaration(self):
        return "const Accessor* other"

    def tidy_lines(self, lines):
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
        super().__init__(function)
        self._owner_class = owner_class

    def tidy_lines(self, lines):
        return lines

    @property
    def body(self):
        # return "\n".join(self.code)
        lines = self.tidy_lines(self.code)
        lines = [self._owner_class.tidy_static_function_line(n) for n in lines]
        return "\n".join(lines)

SPECIALISED_METHODS = {
    ("accessor", "compare"): CompareMethod,
    ("accessor", "dump"): DumpMethod,
}

class Class:
    rename = {}

    substitute_str_top_level = {}
    substitute_re_top_level = {
        r"^#define\s+(\w+)\s+(-?\d+)?!(\.|e|E)": r"const long \1 = \2;",
        r"^#define\s+(\w+)\s+(-?\d+\.\d+([eE]-?\d+)?)": r"const double \1 = \2;",
        r'\bgrib_inline_strcmp\b': 'strcmp',
    }

    def __init__(
        self,
        *,
        path,
        class_,
        functions,
        top_level_code,
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
        self._top_level_code = top_level_code

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
                print(f"[finalise] name={self._name} : ***WARNING*** member {m.name}'s type converted from {self.type_name}* to {m.type}")

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
            m = importlib.import_module(f"patches.{self._cname}")
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

    def cannot_convert_top_level(self, name):
        if name not in self._top_level_code:
            print(list(self._top_level_code.keys()))
            raise Exception(f"Cannot convert top level {name}")
        self._top_level_code[name] = (
            ["#ifdef CANNOT_CONVERT_CODE"] + self._top_level_code[name] + ["#endif"]
        )

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
        return pascal_to_camel(self._name)

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
    def top_level_code(self):
        tidy_top_level_code = defaultdict(list)
        for func_name in self._top_level_code:
            tidy_top_level_code[func_name] = [self.tidy_top_level(line) for line in self._top_level_code[func_name]]
        return tidy_top_level_code
        #return self._top_level_code

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

        name = snake_to_pascal(name)

        #name += "Data"

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
        print(f"\n[dump_header] [IN] {self._name}")
        template = env.get_template(f"{self._class}Data.h.j2")
        self.save("h", template.render(c=self))
        print(f"[dump_header] [IN] {self._name}")

    def dump_body(self):
        print(f"\n[dump_body] [IN] {self._name}")
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
        print(f"[dump_body] [OUT] {self._name}")


    def tidy_line(self, line, this=[]):
        line_b4 = line
        line,count = re.subn(r"\bsuper->\b", f"{self.super}::", line)
        if count:
            print(f"[TIDY] begin [before]: {line_b4}")
            print(f"[TIDY] begin [after ]: {line}")

        accessor_variable_name = pascal_to_camel(self._name)

        for n in this:
            line_b4 = line
            line,count = re.subn(n, f"{accessor_variable_name}", line)
            if count:
                print(f"[TIDY] this [before]: {line_b4}")
                print(f"[TIDY] this [after ]: {line}")

        '''
        if re.match(rf"\bgrib_handle\*", line):
            print(f"[TIDY] deleting: {line}")
            line = ""
        '''

        if re.match(rf".*\b{accessor_variable_name}\s+=", line):
            print(f"[TIDY] deleting: {line}")
            line = ""

        if re.match(rf".*\bsuper\s+=\s+\*\({accessor_variable_name}->cclass->super\)", line):
            print(f"[TIDY] deleting: {line}")
            line = ""

        # Convert all GRIB_ status values
        for k, v in GribStatusConverter.items():
            line_b4 = line
            line, count = re.subn(k, v, line)
            if count:
                print(f"[TIDY] GRIB_STATUS [before]: {line_b4}")
                print(f"[TIDY] GRIB_STATUS [after ]: {line}")

        for k, v in self.substitute_re.items():
            line_b4 = line
            line, count = re.subn(k, v, line)
            if count:
                print(f"[TIDY] substitute_re [before]: {line_b4}")
                print(f"[TIDY] substitute_re [after ]: {line}")

        for m in self.members_in_hierarchy:
            line_b4 = line
            line,count = re.subn(rf"\b{accessor_variable_name}->{m.cname}\b", rf"{m.name}", line)
            if(count):
                print(f"[TIDY] members_in_hierarchy [before]: {line_b4}")
                print(f"[TIDY] members_in_hierarchy [after ]: {line}")

        for m in self._private_methods:
            name = m.name
            line_b4 = line
            line,count = re.subn(rf"\b{name}\s*\(\s*([^,]+),", f"{accessor_variable_name}.{name}(", line)
            if(count):
                print(f"[TIDY] _private_methods [before]: {line_b4}")
                print(f"[TIDY] _private_methods [after ]: {line}")

        m = re.match(r"\s*\breturn\s+(GRIB_\w*)\s*;", line)
        if m:
            if m.group(1).startswith("GRIB_TYPE_"):
                # OK
                pass
            elif m.group(1) != "GRIB_SUCCESS":
                line = f"throw AccessorException({m.group(1)});"

        return line

    def tidy_static_function_line(self, line):
        for m in self._static_functions:
            name = m.name
            line = re.sub(rf"\b{name}\s*\(", f"{snake_to_camel(transform_function_name(name))}(", line)

        return line

    def tidy_top_level(self, line):
        #for k, v in self.substitute_str_top_level.items():
        #    line = line.replace(k, v)

        #for k, v in self.substitute_re_top_level.items():
        #    line = re.sub(k, v, line)

        # Transform any function pointers in top level structs etc
        for m in self._static_functions:
            name = m.name
            line = re.sub(rf"\b{name}\b", f"{snake_to_camel(transform_function_name(name))}", line)

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

    # Note - these substitutions are applied in the order defined below, so dependencies
    #        can be used if required...
    substitute_re = {
        # r'\bgrib_inline_strcmp\b': 'strcmp',
        # r"\bgrib_handle_of_accessor\(this\)": "this->handle()",

        # grib_ functions
        r"\S.*\bgrib_get_string\(.*,\s*(.*),\s*(.*),\s*(.*)\s*\)": r"\2 = toString(\1)",

        # C functions
        r"\bstrcmp\((.*),\s*(.*)\s*\)\s*([!=]=)\s*\d+": r"\1 \3 \2",
        r"\bsnprintf\((.*),\s*(.*),\s*(.*)\s*\)": r"\1 = \3",
        r"\bstrlen\(\s*(.*)\s*\)": r"\1.size()",

        # Variable types
        r"\bchar\s+(.*)\[.*;": r"std::string \1;",

        # Assume sizeof(x)/sizeof(*x) is now a container with a size() member...
        r"\b(.*=\s+)sizeof\((.*)\)\s*/\s*sizeof\(\*\2\);": r"\1\2.size();",

        # Original substitutions - to review
        r"\bGRIB_TYPE_(\w+)": r"GribType::\1",
        r"\bget_accessors\(this\)": "this->get_accessors()",
        r"\bselect_area\(this\)": "this->select_area()",
        r"\bcompute_size\(this\)": "this->compute_size()",
        r"\binit_length\(this\)": "this->init_length()",
        r"\blog_message\(this\)": "this->log_message()",
        r"\bstring_length\(this\)": "this->string_length()",
        r"\bget_accessor\(this\)": "this->get_accessor()",
        r"\bcompute_byte_count\(this\)": "this->compute_byte_count()",
        r"\bselect_datetime\(this\)": "this->select_datetime()",
        r"\bapply_thinning\(this\)": "this->apply_thinning()",
        r"\bconcept_evaluate\(this\)": "this->concept_evaluate()",
        r"\bnew_bif_trunc\(this\)": "this->new_bif_trunc()",
        r"\bget_table_codes\(this\)": "this->get_table_codes()",

        r"\bload_table\(this\)": "this->load_table()",
        r"\bgrib_accessor_get_native_type\(this\)": "this->get_native_type()",
        r"\bget_native_type\(this\)": "this->get_native_type()",
        r"\bgrib_byte_offset\((\w+)\s*\)": r"\1->byte_offset()",
        r"\bgrib_byte_count\((\w+)\s*\)": r"\1->byte_count()",
        r"\bbyte_offset\((\w+)\s*\)": r"\1->byte_offset()",
        r"\bbyte_count\((\w+)\s*\)": r"\1->byte_count()",
        r"\bgrib_pack_string\((\w+)\s*,": r"\1->pack_string(",
        r"\bgrib_pack_double\((\w+)\s*,": r"\1->pack_double(",
        r"\bgrib_unpack_string\((\w+)\s*,": r"\1->unpack_string(",
        r"\bgrib_unpack_double\((\w+)\s*,": r"\1->unpack_double(",
        r"\bgrib_pack_long\((\w+)\s*,": r"\1->pack_long(",
        r"\bgrib_unpack_long\((\w+)\s*,": r"\1->unpack_long(",
        r"\bgrib_unpack_double\((\w+)\s*,": r"\1->unpack_double(",
        r"\bgrib_unpack_bytes\((\w+)\s*,": r"\1->unpack_bytes(",
        r"\bgrib_value_count\((\w+)\s*,": r"\1->value_count(",
        r"\bgrib_update_size\((\w+)\s*,": r"\1->update_size(",
        r"\bgrib_unpack_string_array\((\w+)\s*,": r"\1->unpack_string_array(",
        r"\bgrib_is_missing_internal\((\w+)\)": r"\1->is_missing()",
        r"\bpreferred_size\(this,": r"this->preferred_size(",
        r"\bvalue_count\((\w+),": r"\1->value_count(",
        r"\bDebugAssert\b": "DEBUG_ASSERT",
        r"\bAssert\((.*)\)": r"if (\1) { Assert(false); }",
        #r"\bAssert\b": "ASSERT",
        r"\bpack_double\(this,": "this->pack_double(",
        r"\bunpack_long\(this,": "this->unpack_long(",
        r"\bunpack_double\(this,": "this->unpack_double(",
        r"\bpack_string\(this,": "this->pack_string(",
        r"\bpack_long\(this,": "this->pack_long(",
        r"\bunpack_string\(this,": "this->unpack_string(",
        r"\bpack_bytes\(this,": "this->pack_bytes(",
        r"\bpack_double\(this,": "this->pack_double(",
        r"\bunpack_double_element\(this,": "this->unpack_double_element(",
        r"\bunpack<(\w+)>\(this,": r"this->unpack<\1>(",
        r"\bDBL_MAX\b": "std::numeric_limits<double>::max()",
        r"\bINT_MAX\b": "std::numeric_limits<int>::max()",
        r"\b(\w+)::this->": r"\1::",
        r"\b(\w+)->this->": r"\1::",
        r"\bgrib_accessor\*": "Accessor*",
        r"\bthis->cclass->name\b": "this->className()",
        r"\bgrib_find_accessor\b": "Accessor::find",
        r"\bgrib_pack_long\(this->(\w+),": r"this->\1->pack_long(",
        r"\bgrib_value_count\(this->(\w+),": r"this->\1->value_count(",
        r"\bgrib_pack_bytes\(this->(\w+),": r"this->\1->pack_bytes(",
        r"\bAccessor::find\(this->handle\(\),": r"Accessor::find(",

        # For now...
        # r'\b(\w+)\(this,': r'\1(const_cast<grib_accessor*>(dynamic_cast<const grib_accessor*>(this)),',
        # r'\b(\w+)\(this\)': r'\1(const_cast<grib_accessor*>(dynamic_cast<const grib_accessor*>(this)))',
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
    top_level_lines = []
    top_level_code = defaultdict(list)
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
            top_level_code[function_name] = [x for x in top_level_lines]
            top_level_lines = []

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
                top_level_lines = []
            if 'minmax_val' not in line:
                includes.append(line[9:])
            continue

        if stripped_line.startswith("template "):
            template = stripped_line
            continue

        top_level_lines.append(line)

    if definitions:
        top_level_code["-end-"] = top_level_lines
        class_ = definitions.pop("CLASS")[0]
        accessorImpl = Accessor(
            path=path,
            class_=class_,
            functions=functions,
            top_level_code=top_level_code,
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
