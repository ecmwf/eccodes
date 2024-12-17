#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    class_name = None

    with open(fn, 'r') as f:
        for line in f:
            # match: grib_accessor_gen_t() { class_name_ = "variable"; }
            m = re.match(r'\s*(?P<classname>[\w_]+)\(\) { class_name_ = "(?P<class_name>[\w_]+)"; }', line)
            if m:
                class_name = m.group(2)
                continue

    assert(class_name)

    with open(fn, 'r') as f:
        members = list()
        for line in f:
            m = re.match(r'\s*(?P<class_name>[\w_]+)\(\) { class_name_ = "(?P<type_name>[\w_]+)"; }', line)
            if m:
                out.append(f"        {m.group("class_name")}() {{}}\n")
                out.append(f"    static inline const AccessorType accessor_type{{\"{m.group("type_name")}\"}};\n")
                out.append(f"    const AccessorType& getClassName() const override {{ return accessor_type; }}\n")
                continue

            # match: grib_accessor* create_empty_accessor() override { return new grib_accessor_variable_t{}; }
            m = re.match(r'\s*grib_accessor\* create_empty_accessor\(\) override .*}', line)
            if m:
                continue

            out.append(line)

    return out


for fn in sys.argv[1:]:
    print("Processing", fn)
    out = convert(fn)
    print(''.join(out))
    out_fn = fn.split('/')[-1]
    with open(out_fn, 'w') as f:
        print("Writing to", out_fn)
        f.write(''.join(out))
