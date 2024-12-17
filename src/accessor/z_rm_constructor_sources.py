#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = []
    with open(fn, 'r') as f:
        for line in f:
            m = re.match(r'.*grib_accessor_(?P<bname>\w*)_t::grib_accessor_(?P<dname>\w*)_t\(\)\s*:', line)
            if m:
                while not re.match(r'.*{}$', line):
                    line = f.readline()
                    continue
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
