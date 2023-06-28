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
parser.add_argument("--target", default="converted")
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


def to_camel_class(s):
    return "".join(x.capitalize() for x in s.lower().split("_"))


def to_camel_member(s):
    s = to_camel_class(s)
    return s[0].lower() + s[1:]


class Member:
    def __init__(self, s) -> None:
        bits = s.split()
        self.type = " ".join(bits[:-1])

        if "[" in bits[-1]:
            self.name, self._array = bits[-1].split("[")
            self._array = "[" + self._array
        else:
            self.name = bits[-1]
            self._array = ""

        self.name = self.name + "_"

        if self.name[0] == "*":
            self.name = self.name[1:]
            self.type += "*"

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
        self._args = []
        self._lines = []

        for arg in [a.strip() for a in args.split(",")]:
            if not arg:
                continue
            bits = arg.split()
            type = " ".join(bits[:-1])
            name = bits[-1]
            self._args.append(Arg(name, type))

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
    def args_declaration(self):
        return ", ".join([f"{a.type} {a.name}" for a in self._args])

    @property
    def call_args(self):
        return ", ".join([a.name for a in self._args])

    @property
    def args(self):
        return self._args

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
            self._lines.insert(-1, "throw EccodesException(GRIB_NOT_IMPLEMENTED);")


class FunctionDelegate:
    def __init__(self, function):
        self._function = function

    def __getattr__(self, name):
        return getattr(self._function, name)


class Method(FunctionDelegate):
    def __init__(self, owner_class, function, const):
        super().__init__(function)
        self._owner_class = owner_class
        self._const = const

    @property
    def args_declaration(self):
        return ", ".join([f"{a.type} {a.name}" for a in self._args[1:]])

    @property
    def call_args(self):
        return ", ".join([a.name for a in self._args[1:]])

    @property
    def const(self):
        return "const" if self._const else ""

    def tidy_lines(self, lines):
        return lines

    @property
    def body(self):
        this = [r"\bself\b"]
        # Look for this-> or self-> or a->
        ptr_type_name = self._owner_class.type_name + "*"
        if len(self.args) > 0:
            arg = self.args[0]
            if arg.type == ptr_type_name:
                this.append(rf"\b{arg.name}\b")
        lines = self.tidy_lines(self.code)
        lines = [self._owner_class.tidy_line(n, this) for n in lines]
        return "\n".join(lines)


class InheritedMethod(Method):
    pass


class PrivateMethod(Method):
    pass


class DestructorMethod(Method):
    @property
    def body(self):
        lines = ["grib_context* c = 0;", super().body]
        return "\n".join(lines)


class ConstructorMethod(Method):
    pass


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
            line = re.sub(r"\bother->(\w+),", r"other->\1_,", line)
            result.append(line)

        return result


class DumpMethod(Method):
    @property
    def body(self):
        return "\n".join(
            ["#if 0"]
            + self.code
            + ["#endif", "throw EccodesException(GRIB_NOT_IMPLEMENTED);"]
        )


class StaticProc(FunctionDelegate):
    def __init__(self, owner_class, function):
        super().__init__(function)
        self._owner_class = owner_class

    @property
    def body(self):
        return "\n".join(self.code)


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

        # Classify functions
        self._inherited_methods = []
        self._private_methods = []
        self._static_functions = []

        # Overwitten functions
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
        ptr_type_name = self.type_name + "*"
        for name, f in list(self._functions.items()):
            if f._return_type == ptr_type_name:
                f._return_type = self.class_name + "*"

            # If starts with ptr_type_name, then it's a private method
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
        return self._top_level_code

    @property
    def members(self):
        return self._members

    @property
    def members_names(self):
        result = set(self.top_members)
        for m in self._members:
            result.add(m.name)
        if self.super in self._other_classes:
            result.update(self._other_classes[self.super].members_names)

        return sorted(result)

    @property
    def include_super(self):
        return "/".join([self._include_dir] + self.namespaces + [self._super + ".h"])

    @property
    def include_header(self):
        return "/".join([ARGS.target] + self.namespaces + [self._name + ".h"])

    @property
    def header_includes(self):
        return []

    @property
    def body_includes(self):
        return self._body_includes

    @property
    def namespaces(self):
        return ["eccodes", self._class]

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

        name = to_camel_class(name)
        return self.rename.get(name, name), c_name

    def save(self, ext, content):
        target = os.path.join(ARGS.target, *self.namespaces, f"{self.name}.{ext}")
        LOG.info("Writting %s", target)

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
        template = env.get_template(f"{self._class}.h.j2")
        self.save("h", template.render(c=self))

    def dump_body(self):
        # Beware of this: https://github.com/pallets/jinja/issues/604
        template = env.get_template(f"{self._class}.cc.j2")

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

    def tidy_line(self, line, this=[]):
        line = re.sub(r"\bsuper->\b", f"{self.super}::", line)

        for n in this:
            line = re.sub(n, "this", line)

        if re.match(r".*\bthis\s+=", line):
            line = ""

        if re.match(r".*\bsuper\s+=\s+\*\(this->cclass->super\)", line):
            line = ""

        for k, v in self.substitute_re.items():
            line = re.sub(k, v, line)

        for m in self.members_names:
            name = m[:-1]
            line = re.sub(rf"\bthis->{name}\b", rf"this->{name}_", line)

        for m in self._private_methods:
            name = m.name
            line = re.sub(rf"\b{name}\s*\(\s*([^,]+),", f"this->{name}(", line)

        m = re.match(r"\s*\breturn\s+(GRIB_\w*)\s*;", line)
        if m and m.group(1) != "GRIB_SUCCESS":
            # e = to_camel_class(m.group(1))
            # line = f"throw {e}Exception();"
            line = f"throw EccodesException({m.group(1)});"

        return line

    def tidy_top_level(self, line):
        for k, v in self.substitute_str_top_level.items():
            line = line.replace(k, v)

        for k, v in self.substitute_re_top_level.items():
            line = re.sub(k, v, line)

        return line


class Accessor(Class):
    class_name = "Accessor"
    type_name = "grib_accessor"
    constructor_args = "grib_accessor* a, const long l, grib_arguments* c"

    top_members = [
        "name_",
        "length_",
        "offset_",
        "dirty_",
        "flags_",
        "context_",
        "parent_",
        "next_",
        "h_",
    ]

    non_const_methods = [
        "pack_long",
        "pack_missing",
        "pack_string",
        "pack_double",
        "pack_bytes",
        "pack_expression",
        "pack_string_array",
        "update_size",
    ]

    # namespaces = ["eccodes", "accessor"]
    prefix = "grib_accessor_class_"
    rename = {
        "Gen": "Generic",
        "Md5": "Md5Sum",  # We rename because of macos case insensitive file system
        "Assert": "Assertion", # Name clash with assert.h
    }

    substitute_re = {
        r'\bgrib_inline_strcmp\b': 'strcmp',
        r"\bgrib_handle_of_accessor\(this\)": "this->handle()",
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
        r"\bAssert\b": "ASSERT",
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
        r"\bdirty\b": "dirty_",
        r"\b(\w+)->length\b": r"\1->length_",
        r"\b(\w+)->offset\b": r"\1->offset_",
        r"\bgrib_find_accessor\b": "Accessor::find",
        r"\bgrib_pack_long\(this->(\w+),": r"this->\1->pack_long(",
        r"\bgrib_value_count\(this->(\w+),": r"this->\1->value_count(",
        r"\bgrib_pack_bytes\(this->(\w+),": r"this->\1->pack_bytes(",
        r"\bAccessor::find\(this->handle\(\),": r"Accessor::find(",
        # Temp stuff for making sure we still compile
        r"\bgrib_optimize_decimal_factor\(this,": r"grib_optimize_decimal_factor(this->as_grib_accessor_while_converting(),",
        r"\bpack_long_unsigned_helper\(this,": r"pack_long_unsigned_helper(this->as_grib_accessor_while_converting(),",
        r"\bgrib_g1_step_get_steps\(this,": r"grib_g1_step_get_steps(this->as_grib_accessor_while_converting(),",
        r"\bfree_bif_trunc\(bt, this\)": r"free_bif_trunc(bt, this->as_grib_accessor_while_converting())",
        r"\badd_many_bitstream\(&ctx, this,": r"add_many_bitstream(&ctx, this->as_grib_accessor_while_converting(),",
        r"\badd_bitstream\(&ctx, this,": r"add_bitstream(&ctx, this->as_grib_accessor_while_converting(),",
        r"\baccessor_raw_get_offset\(this->(\w+)": r"accessor_raw_get_offset(this->\1->as_grib_accessor_while_converting()",
        r"\bgrib_accessor_class_expanded_descriptors_set_do_expand\((\w+)": r"grib_accessor_class_expanded_descriptors_set_do_expand(\1->as_grib_accessor_while_converting()",

        r"\bgrib_is_missing_string\((\w+)": r"grib_is_missing_string(\1->as_grib_accessor_while_converting()",
        r"\bgrib_is_missing_long\((\w+)": r"grib_is_missing_long(\1->as_grib_accessor_while_converting()",
        r"\bgrib_is_missing_double\((\w+)": r"grib_is_missing_double(\1->as_grib_accessor_while_converting()",
r"\baccessor_bufr_data_array_set_unpackMode\((\w+)": r"accessor_bufr_data_array_set_unpackMode(\1->as_grib_accessor_while_converting()",

r"\baction_concept_get_nofail\((\w+)": r"action_concept_get_nofail(\1->as_grib_accessor_while_converting()",
r"\baction_concept_get_concept\((\w+)": r"action_concept_get_concept(\1->as_grib_accessor_while_converting()",

    }

    def class_to_type(self):
        return self.cname.replace("_class_", "_")


class Iterator(Class):
    pass


class Action(Class):
    pass


class Box(Class):
    pass


class Dumper(Class):
    pass


class Expression(Class):
    pass


class Nearest(Class):
    pass


CLASSES = dict(
    accessor=Accessor,
    iterator=Iterator,
    expression=Expression,
    nearest=Nearest,
    action=Action,
    box=Box,
    dumper=Dumper,
)


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
        klass = CLASSES[class_](
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
        return klass


def main():
    classes = {}
    for a in ARGS.path:
        klass = parse_file(a)
        if klass is not None:
            classes[klass.name] = klass

    LOG.info("Finalising %s classes", len(classes))
    for klass in classes.values():
        klass.finalise(classes)

    for klass in classes.values():
        klass.dump()


if __name__ == "__main__":
    main()
