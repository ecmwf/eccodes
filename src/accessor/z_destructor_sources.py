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
            # match: void grib_accessor_unsigned_t::destroy(grib_context* context)
            m = re.match(r'(?P<rtype>\w+)\s+grib_accessor_(?P<name>\w+)_t::destroy\((?P<args>.*)\)(?P<after>.*)', line)
            if m:
                out.append(f"grib_accessor_{m.group('name')}_t::~grib_accessor_{m.group('name')}_t()\n")
                continue

            # match: grib_accessor_long_t::destroy(context);
            m = re.match(r'\s*grib_accessor_(?P<name>\w+)_t::destroy\((?P<args>.*)\);', line)
            if m:
                continue


            # match: grib_context_free(c, v_);
            m = re.match(r'\s*grib_context_free\(\s*c\s*,\s*(?P<args>.*)\);', line)
            if m:
                out.append(f"    grib_context_free(context_, {m.group('args')});\n")
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
