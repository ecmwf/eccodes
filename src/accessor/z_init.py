#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()

    with open(fn, 'r') as f:
        for line in f:
            
            # const char* seclen_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>const\s+char\s*\*)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = nullptr;{m.group('rest')}\n")
                continue

            m = re.match(r'(?P<spaces>\s*)(?P<type>char\s*\*)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = nullptr;{m.group('rest')}\n")
                continue

            m = re.match(r'(?P<spaces>\s*)(?P<type>grib_handle\s*\*)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = nullptr;{m.group('rest')}\n")
                continue

            m = re.match(r'(?P<spaces>\s*)(?P<type>\w+\s*\*)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = nullptr;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>int)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>size_t)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>unsigned long)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>char)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>double)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0.;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>float)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0.f;{m.group('rest')}\n")
                continue

            # parse int values_dirty_;
            m = re.match(r'(?P<spaces>\s*)(?P<type>long)\s+(?P<member>\w+)_;(?P<rest>\s*.*)\n', line)
            if m:
                out.append(f"{m.group('spaces')}{m.group('type')} {m.group('member')}_ = 0;{m.group('rest')}\n")
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
