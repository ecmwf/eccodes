#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    cname = None
    super_cname = None

    with open(fn, 'r') as f:
        for line in f:
            # match: grib_accessor_gds_is_present_t();
            m = re.match(r'.*grib_accessor_(?P<name>\w*)_t\(\);', line)
            if m:
                cname = m.group('name')
                continue



    assert(cname is not None)

    with open(fn, 'r') as f:
        members = list()
        for line in f:
            # match:  grib_accessor_variable_t();
            m = re.match(r'\s*grib_accessor_(?P<name>\w*)_t\(\);', line)
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
