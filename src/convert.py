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


class Method:
    def __init__(self, name, result, args) -> None:
        self.name = name

        self.result = result
        self.lines = []

        args = [a.strip() for a in args.split(",")]
        self.args = ", ".join(args[1:])
        self.args_list = []
        for arg in [re.sub(r"\s+", " ", a).strip() for a in args]:
            bits = arg.split()
            type = " ".join(bits[:-1])
            name = bits[-1]
            self.args_list.append((type, name))

    def add_line(self, line):
        self.lines.append(line)

    @property
    def body(self):
        return "\n".join(self.lines)

    def tidy_lines(self, klass):
        this = [r"\bself\b"]
        if self.args_list:
            type, name = self.args_list[0]
            # Check if the first argument is a pointer to the class
            if type == klass.type_name + "*":
                if re.match(r"^\w+$", name):
                    this.append(rf"\b{name}\b")
        self.lines = [klass.tidy_line(n, this) for n in self.lines[1:-1]]

    def has_this(self):
        for line in self.lines:
            if "this->" in line:
                return True
        return False


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
        inherited_procs,
        other_procs,
        top_level,
        includes,
        factory_name,
        SUPER=[],
        IMPLEMENTS=[],
        MEMBERS=[],
    ):
        assert factory_name is not None

        self.class_ = class_
        self.name, self.cname = self.tidy_class_name(path)
        self.inherited_procs = inherited_procs
        self.factory_name = factory_name

        self.self = None
        self.a = None
        self.header_includes = []
        self.body_includes = includes
        if SUPER:
            self.super, _ = self.tidy_class_name(SUPER[0])
        else:
            self.super, _ = self.tidy_class_name(class_)

        self.members = [Member(m) for m in MEMBERS if m != ""]

        for p in inherited_procs.values():
            p.tidy_lines(self)

        self.inherited_methods = [
            p
            for p in inherited_procs.values()
            if p.name
            not in (
                "init",
                "destroy",
            )
        ]
        init = [p for p in inherited_procs.values() if p.name == "init"]
        self.constructor = init[0] if init else Method("init", "void", "void")

        init = [p for p in inherited_procs.values() if p.name == "destroy"]
        self.destructor = init[0] if init else Method("init", "void", "void")

        for p in other_procs.values():
            p.tidy_lines(self)

        self.private_methods = [p for p in other_procs.values() if p.has_this()]
        self.static_procs = [p for p in other_procs.values() if not p.has_this()]

        self.top_level = defaultdict(list)
        for k, v in top_level.items():
            self.top_level[k] = [self.tidy_top_level(p) for p in v]

    def dump(self):
        self.dump_header()
        self.dump_body()

    def tidy_class_name(self, path):
        path, ext = os.path.splitext(path)
        cname = os.path.basename(path)

        name = cname.replace(self.prefix, "")

        name = to_camel_class(name)
        return self.rename.get(name, name), cname

    def save(self, ext, content):
        target = os.path.join(args.target, *self.namespaces, f"{self.name}.{ext}")
        os.makedirs(os.path.dirname(target), exist_ok=True)
        with open(target, "w") as f:
            f.write(content)

        ret = os.system(f"clang-format -i {target}")
        assert ret == 0
        print("      ", target)

    def dump_header(self):
        template = env.get_template(f"{self.class_}.h.j2")
        self.save(
            "h",
            template.render(
                name=self.name,
                super=self.super,
                members=self.members,
                includes=self.header_includes,
                inherited_methods=self.inherited_methods,
                private_methods=self.private_methods,
                static_procs=self.static_procs,
                constructor=self.constructor,
                destructor=self.destructor,
                namespaces=self.namespaces,
                namespace_reversed=reversed(self.namespaces),
                include_super="/".join(self.namespaces + [f"{self.super}.h"]),
            ),
        )

    def dump_body(self):
        template = env.get_template(f"{self.class_}.cc.j2")

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
            tidy_more(
                template.render(
                    name=self.name,
                    super=self.super,
                    members=self.members,
                    includes=self.body_includes,
                    inherited_methods=self.inherited_methods,
                    private_methods=self.private_methods,
                    static_procs=self.static_procs,
                    constructor=self.constructor,
                    destructor=self.destructor,
                    namespaces=self.namespaces,
                    namespace_reversed=reversed(self.namespaces),
                    include_header="/".join(self.namespaces + [f"{self.name}.h"]),
                    top_level=self.top_level,
                    factory_name=self.factory_name,
                )
            ),
        )

    def tidy_line(self, line, this):
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
        r"\bDebugAssert\b": "ASSERT",
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

def make_class(path):
    in_def = False
    in_imp = False
    in_proc = False
    includes = []
    factory_name = None

    definitions = {}
    inherited_procs = {}
    other_procs = {}
    top_level_lines = []
    top_level = defaultdict(list)
    print(path, file=sys.stderr)

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
            p = m.group(2)
            top_level[p] = [x for x in top_level_lines]
            top_level_lines = []

            if p in definitions.get("IMPLEMENTS", []):
                in_proc = True
                proc = inherited_procs[p] = Method(p, m.group(1), m.group(3))
                depth = stripped_line.count("{") - stripped_line.count("}")
                assert depth >= 0, line
                continue
            else:
                in_proc = True
                proc = Method(p, m.group(1), m.group(3))
                other_procs[p] = proc
                depth = stripped_line.count("{") - stripped_line.count("}")
                assert depth >= 0, line
                continue

        if in_proc:
            proc.add_line(stripped_line)
            depth += stripped_line.count("{")
            depth -= stripped_line.count("}")
            assert depth >= 0, line
            if depth == 0:
                in_proc = False
                del proc
            continue

        if stripped_line.startswith("#include"):
            if len(includes) == 0:
                # Forget lines before the first include
                top_level_lines = []
            includes.append(line[9:])
            continue

        top_level_lines.append(line)

    if definitions:
        top_level["-end-"] = top_level_lines
        class_ = definitions.pop("CLASS")[0]
        klass = CLASSES[class_](
            path=path,
            class_=class_,
            inherited_procs=inherited_procs,
            other_procs=other_procs,
            top_level=top_level,
            includes=includes,
            factory_name=factory_name,
            **definitions,
        )
        klass.dump()


def main():
    for a in args.path:
        make_class(a)


if __name__ == "__main__":
    main()
