#!/usr/bin/env python3

import sys
import re

def get_block(f):
    out = []
    for line in f:
        out.append(line)
        if re.match(r'}', line):
            break
    return out

def convert(fn):
    out = []
    with open(fn, 'r') as f:
        for line in f:
            # match: grib_accessor_variable_t _grib_accessor_variable{};
            m = re.match(r'\s*(?P<type>[\w_]+) (?P<name>[\w_]+){};', line)
            if m:
                out.append(f'AccessorBuilder<{m.group("type")}> {m.group("name")}_builder{{}};\n')
                continue

            # match: grib_accessor* grib_accessor_variable = &_grib_accessor_variable;
            m = re.match(r'\s*(?P<type>[\w_]+)\* (?P<name>[\w_]+) = &(?P<var>[\w_]+);', line)
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
        f.write(''.join(out))
