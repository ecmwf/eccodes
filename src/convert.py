#!/usr/bin/env python3

import argparse
import os
import re
import sys
from collections import defaultdict

from jinja2 import Environment, FileSystemLoader, StrictUndefined

parser = argparse.ArgumentParser()
parser.add_argument("--templates", default="j2")
parser.add_argument("--target", default="converted")
parser.add_argument("path", nargs="+")

args = parser.parse_args()

env = Environment(
    loader=FileSystemLoader(args.templates),
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
        self.name = bits[-1] + "_"

        if self.name[0] == "*":
            self.name = self.name[1:]
            self.type += "*"


class Arg:
    def __init__(self, name, type) -> None:
        self.name = name
        self.type = type

        if self.name[0] == "*":
            self.name = self.name[1:]
            self.type += "*"


class Function:
    def __init__(self, name, result, args, template=None) -> None:
        self._name = name
        self._result = result
        self._template = template
        self._args = []
        self._lines = []

        for arg in [a.strip() for a in args.split(",")]:
            bits = arg.split()
            type = " ".join(bits[:-1])
            name = bits[-1]
            self._args.append(Arg(name, type))

    def add_line(self, line):
        self._lines.append(line)

    @property
    def args_declaration(self):
        return ", ".join([f"{a.type} {a.name}" for a in self._args])

    @property
    def call_args(self):
        return ", ".join([a.name for a in self._args])

    @property
    def body(self):
        return ""
        return "\n".join(self._lines)


class Method:
    def __init__(self, name, result, args, template=None) -> None:
        self.name = name

        self.result = result
        self.lines = []

        args = [a.strip() for a in args.split(",")]
        self.full_args = ", ".join(args)
        self.args = ", ".join(args[1:])
        self.args_list = []
        for arg in [re.sub(r"\s+", " ", a).strip() for a in args]:
            bits = arg.split()
            type = " ".join(bits[:-1])
            name = bits[-1]
            self.args_list.append((type, name))

        self.call_args = ", ".join([n for _, n in self.args_list[1:]])
        self.template = template

    def add_line(self, line):
        self.lines.append(line)

    @property
    def body(self):
        return "\n".join(self.lines)

    def has_this(self):
        return True


class SimpleMethod(Method):
    def tidy_lines(self, klass):
        this = [r"\bself\b"]
        if self.args_list and self.template is None:
            type, name = self.args_list[0]
            # Check if the first argument is a pointer to the class
            if type == klass.type_name + "*":
                if re.match(r"^\w+$", name):
                    this.append(rf"\b{name}\b")
        self.lines = [klass.tidy_line(n, this) for n in self.lines[1:-1]]


class CompareMethod(Method):
    def tidy_lines(self, klass):
        assert len(self.args_list) == 2
        assert self.args_list[0][0] == "grib_accessor*"
        assert self.args_list[1][0] == "grib_accessor*"

        a = self.args_list[0][1]
        b = self.args_list[1][1]

        lines = []
        for line in self.lines[1:-1]:
            line = re.sub(rf"\b{a}\b", "this", line)
            line = re.sub(rf"\b{b}\b", "other", line)
            line = klass.tidy_line(line, [])
            line = re.sub(r"\bother->(\w+),", r"other->\1_,", line)
            lines.append(line)

        self.lines = lines
        self.args_list[0] = ("Accessor*", "this")
        self.args_list[1] = ("const Accessor*", "other")
        self.args = "const Accessor* other"


class DumpMethod(Method):
    def tidy_lines(self, klass):
        # For now, just remove the method
        self.lines = ["#if 0"] + self.lines[1:-1] + ["#endif"]


class StaticProc(Method):
    def has_this(self):
        if self.template is not None:
            return False
        for line in self.lines:
            if "this->" in line:
                return True
        return False

    def tidy_lines(self, klass):
        self.lines = [klass.tidy_line(n, []) for n in self.lines[1:-1]]


METHOD_CLASS = {
    "compare": CompareMethod,
    "dump": DumpMethod,
}


class Class:
    rename = {}

    substitute_str_top_level = {}
    substitute_re_top_level = {
        r"^#define\s+(\w+)\s+(-?\d+)?!(\.|e|E)": r"const long \1 = \2;",
        r"^#define\s+(\w+)\s+(-?\d+\.\d+([eE]-?\d+)?)": r"const double \1 = \2;",
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
        self._name, self.cname = self.tidy_class_name(path)
        self._top_level_code = top_level_code
        self._factory_name = factory_name
        self._members = members
        self._functions = functions

        self._body_includes = includes

        if super is None:
            self._super, _ = self.tidy_class_name(class_)
            self._include_dir = "cpp"
        else:
            self._super, _ = self.tidy_class_name(super)
            self._include_dir = args.target

    def finalise(self, other_classes):
        self._other_classes = other_classes

        # Classify functions
        self._inherided_methods = []
        self._private_methods = []
        self._static_functions = []


    @property
    def name(self):
        return self._name

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
        # if self.super in self.other_classes:
        #     return self.other_classes[self.super].members + self._members
        return self._members

    @property
    def include_super(self):
        return "/".join([self._include_dir] + self.namespaces + [self._super + ".h"])

    @property
    def include_header(self):
        return "/".join([args.target] + self.namespaces + [self._super + ".h"])

    @property
    def header_includes(self):
        return []

    @property
    def body_includes(self):
        return []

    @property
    def namespaces(self):
        return ["eccodes", self._class]

    @property
    def namespaces_reversed(self):
        return reversed(self.namespaces)

    @property
    def constructor(self):
        if "init" in self._functions:
            return self._functions["init"]

        return Function("a")

    @property
    def destructor(self):
        if "destroy" in self._functions:
            return self._functions["destroy"]

        return Function("a", "b", "c")

    @property
    def inherited_methods(self):
        return []

    @property
    def private_methods(self):
        return []

    @property
    def static_functions(self):
        return []

        # if SUPER:
        #     self.super, _ = self.tidy_class_name(SUPER[0])
        #     self.src = args.target
        # else:
        #     self.super, _ = self.tidy_class_name(class_)
        #     self.src = "cpp"

        # self.members = [Member(m) for m in MEMBERS if m != ""]

        # for p in inherited_procs.values():
        #     p.tidy_lines(self)

        # self.inherited_methods = [
        #     p
        #     for p in inherited_procs.values()
        #     if p.name
        #     not in (
        #         "init",
        #         "destroy",
        #     )
        # ]
        # init = [p for p in inherited_procs.values() if p.name == "init"]
        # self.constructor = (
        #     init[0]
        #     if init
        #     else SimpleMethod(
        #         "init",
        #         "void",
        #         "grib_accessor* a, const long length, grib_arguments* args",
        #     )
        # )

        # init = [p for p in inherited_procs.values() if p.name == "destroy"]
        # self.destructor = init[0] if init else SimpleMethod("destroy", "void", "void")

        # for p in other_procs.values():
        #     p.tidy_lines(self)

        # self.private_methods = [p for p in other_procs.values() if p.has_this()]
        # self.static_procs = [p for p in other_procs.values() if not p.has_this()]

        # self.top_level = defaultdict(list)
        # for k, v in top_level.items():
        #     self.top_level[k] = [self.tidy_top_level(p) for p in v]

    def update(self, classes):
        if self.super in classes:
            self.members += classes[self.super].members

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
        target = os.path.join(args.target, *self.namespaces, f"{self.name}.{ext}")
        os.makedirs(os.path.dirname(target), exist_ok=True)
        print("Writting ", target)
        with open(target, "w") as f:
            f.write(content)

        ret = os.system(f"clang-format -i {target}")
        assert ret == 0

    def dump_header(self):
        template = env.get_template(f"{self._class}.h.j2")
        self.save(
            "h",
            template.render(c=self),
        )

    def dump_body(self):
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

        self.save(
            "cc",
            tidy_more(template.render(c=self)),
        )

    def tidy_line(self, line, this=[]):
        line = re.sub(r"\bsuper->\b", f"{self.super}::", line)

        for n in this:
            line = re.sub(n, "this", line)

        if re.match(r".*\bthis\s+=", line):
            line = ""

        if re.match(r".*\bsuper\s+=\s+\*\(this->cclass->super\)", line):
            line = ""

        for k, v in self.substitute_str.items():
            line = line.replace(k, v)

        for k, v in self.substitute_re.items():
            line = re.sub(k, v, line)

        for m in self.members:
            name = m.name[:-1]
            line = re.sub(rf"\bthis->{name}\b", rf"this->{name}_", line)

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
    type_name = "grib_accessor"

    namespaces = ["eccodes", "accessor"]
    prefix = "grib_accessor_class_"
    rename = {"Gen": "Generic"}

    substitute_str = {
        "grib_handle_of_accessor(this)": "this->handle()",
    }
    substitute_re = {
        r"\bthis->length\b": "this->length_",
        r"\bthis->offset\b": "this->offset_",
        r"\bthis->flags\b": "this->flags_",
        r"\bthis->context\b": "this->context_",
        r"\bgrib_byte_offset\((\w+)\s*\)": r"\1->byte_offset()",
        r"\bgrib_byte_count\((\w+)\s*\)": r"\1->byte_count()",
        r"\bgrib_pack_string\((\w+)\s*,": r"\1->pack_string(",
        r"\bgrib_pack_long\((\w+)\s*,": r"\1->pack_long(",
        r"\bgrib_unpack_long\((\w+)\s*,": r"\1->unpack_long(",
        r"\bgrib_value_count\((\w+)\s*,": r"\1->value_count(",
        r"\bpreferred_size\(this,": r"this->preferred_size(",
        r"\bDebugAssert\b": "DEBUG_ASSERT",
        r"\bAssert\b": "ASSERT",
        r"\bunpack_long\(this,": "this->unpack_long(",
        r"\bDBL_MAX\b": "std::numeric_limits<double>::max()",
        r"\bINT_MAX\b": "std::numeric_limits<int>::max()",
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
    in_def = False
    in_imp = False
    in_function = False
    includes = []
    factory_name = None
    template = None

    definitions = {}
    functions = {}
    top_level_lines = []
    top_level_code = defaultdict(list)
    print("Parsing", path)

    f = open(path, "r")
    for line in f:
        stripped_line = line.strip()
        line = line.rstrip()

        if stripped_line.startswith("START_CLASS_DEF"):
            in_def = True
            continue

        if stripped_line.startswith("END_CLASS_DEF"):
            in_def = False
            continue

        if stripped_line.startswith("/* START_CLASS_IMP */"):
            in_imp = True
            continue

        if stripped_line.startswith("/* END_CLASS_IMP */"):
            in_imp = False
            continue

        if in_imp:
            m = re.match(r"\s*\"(\w+)\",\s+/\* name \*/", stripped_line)
            if m:
                factory_name = m.group(1)
            continue

        if in_def:
            if stripped_line.strip() == "":
                continue
            bits = [s.strip() for s in re.split(r"[=;]+", stripped_line)]
            try:
                what = bits[0]
                definitions.setdefault(what, [])
                definitions[what] += bits[1:]
            except KeyError:
                print(f"Unknown definition: {bits}")
                raise
            continue

        m = re.match(r"static\s+([^(]+)\s+(\w+)\s*\(([^(]+)\)", line)
        if m:
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

        if in_function:
            function.add_line(stripped_line)
            depth += stripped_line.count("{")
            depth -= stripped_line.count("}")
            assert depth >= 0, line
            if depth == 0:
                in_function = False
                template = None
                del function
            continue

        if stripped_line.startswith("#include"):
            if len(includes) == 0:
                # Forget lines before the first include
                top_level_lines = []
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
    for a in args.path:
        klass = parse_file(a)
        if klass is not None:
            classes[klass.cname] = klass

    for klass in classes.values():
        klass.finalise(classes)

    for klass in classes.values():
        klass.dump()


if __name__ == "__main__":
    main()
