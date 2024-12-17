#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    with open(fn, 'r') as f:
        members = list()
        for line in f:
            # grib_accessor_unsigned_t* self = (grib_accessor_unsigned_t*)a;
            m = re.match(r'\s*grib_accessor_(?P<aname>\w+)_t\*\s+self\s=\s*.*', line) 
            if m:
                continue

            m1 = re.match(r'static (?P<return_type>\w+) (?P<function_name>\w+)\((?P<parameters>.+)\)', line)
            m = re.match(r'(?P<before>.*)\s*self->(?P<name>\w+)\s*(?P<after>.*)', line) 
            if m:
                out.append(f"{m.group('before')}self->{m.group('name')}_ {m.group('after')}\n")
                continue

            m = re.match(r'(?P<before>.*)\s*a->(?P<name>\w+\()\s*(?P<after>.*)', line) 
            if m:
                out.append(f"{m.group('before')}{m.group('name')}{m.group('after')}\n")
                continue

            m = re.match(r'(?P<before>.*)\s*a->(?P<name>\w+)\s*(?P<after>.*)', line) 
            if m:
                out.append(f"{m.group('before')}{m.group('name')}_ {m.group('after')}\n")
                continue

            # void grib_accessor_class_unsigned_t::init(grib_accessor* a)
            m = re.match(r'(?P<rtype>\w+)\s+grib_accessor_class_(?P<aname>\w+)_t::(?P<fname>\w+)\(grib_accessor\* a\)(?P<after>.*)', line) 
            if m:
                out.append(f"{m.group('rtype')} grib_accessor_{m.group('aname')}_t::{m.group('fname')}(){m.group('after')}\n")
                continue


            # void grib_accessor_class_unsigned_t::init(grib_accessor* a, const long len, grib_arguments* arg)
            m = re.match(r'(?P<rtype>\w+)\s+grib_accessor_class_(?P<aname>\w+)_t::(?P<fname>\w+)\(grib_accessor. a, (?P<after>.*)', line) 
            if m:
                out.append(f"{m.group('rtype')} grib_accessor_{m.group('aname')}_t::{m.group('fname')}({m.group('after')}\n")
                continue

            # grib_accessor_class_unsigned_t _grib_accessor_class_unsigned("unsigned");
            m = re.match(r'grib_accessor_class_(?P<aname>\w+)_t\s+_grib_accessor_class_(?P<aname2>\w+){\s*"(?P<class_name>.*)"\s*}\s*;', line) 
            if m:
                out.append(f"grib_accessor_{m.group('aname')}_t _grib_accessor_{m.group('aname2')}{{}};\n")
                continue

            # grib_accessor_class* grib_accessor_class_unsigned = &_grib_accessor_class_unsigned;
            m = re.match(r'grib_accessor_class\*\s+grib_accessor_class_(?P<aname>\w+)\s*=\s*&_grib_accessor_class_(?P<aname3>\w+)(?P<after>.*)', line) 
            if m:
                out.append(f"grib_accessor* grib_accessor_{m.group('aname')} = &_grib_accessor_{m.group('aname')}{m.group('after')}\n")
                continue

            out.append(line)


    prev = None
    out2 = list()
    for line in out:
        if line == prev:
            continue
        out2.append(line)
        prev = line

    # for line in out2:
    #     print(line, end='')
    return out2


for fn in sys.argv[1:]:
    print("Processing", fn)
    out = convert(fn)
    print(''.join(out))

    out_fn = fn.split('/')[-1]
    with open(out_fn, 'w') as f:
        f.write(''.join(out))
