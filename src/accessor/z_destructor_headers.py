#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    class_name = None
    name = None

    with open(fn, 'r') as f:
        for line in f:
            #match: grib_accessor_double_t() { class_name_ = "abstract_vector"; }
            #match: grib_accessor_variable_t();
            m = re.match(r'.*grib_accessor_(?P<name>\w*)_t\(\).*', line)
            if m:
                name = m.group('name')
                break

    assert(name)

    with open(fn, 'r') as f:
        members = list()
        for line in f:
            # match void destroy(grib_context*) override;
            m = re.match(r'.*void\s*destroy\(\s*grib_context\*\s*\)\s*override;\s*', line)
            if m:
                # add a destructor
                out.append(f'    virtual ~grib_accessor_{name}_t();\n')
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
