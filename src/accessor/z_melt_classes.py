#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    class_name = None
    
    ## Replace extension .h with .cc
    fn_cc = fn.replace('.h', '.cc')
    with open(fn_cc, 'r') as f:
        for line in f:
            # grib_accessor_class_validity_time_t _grib_accessor_class_validity_time{ "validity_time" };
            m = re.match(r'.*grib_accessor_class_[a-zA-Z0-9_]*_t\s*_grib_accessor_class_[a-zA-Z0-9_]*\s*{\s*"(?P<class_name>.*)"\s*};.*', line)
            if m:
                print("class_name:", m.group('class_name'))
                class_name = m.group('class_name')
                break

    # if not class_name:
    #     print("Error: Could not find class_name in", fn)
    #     return

    print("class_name:", class_name)


    with open(fn, 'r') as f:
        members = list()
        for line in f:
            if re.match(r'class grib_accessor_class[a-zA-Z0-9]*', line):
                while not re.match(r'public:', line):
                    out.append(line)
                    line = next(f)
                while not re.match(r'};', line):
                    out.append(line)
                    line = next(f)
                if members:
                    out.append("private:\n")
                    for member in members:
                        out.append(member)

            # class grib_accessor_constant_t : public grib_accessor_variable_t {};
            if re.match(r'class grib_accessor_.*{};', line):
                print("Skipping", line)
                continue

            if re.match(r'class grib_accessor_[a-zA-Z0-9]*', line):
                while not re.match(r'public:', line):
                    line = next(f)
                line = next(f)
                while not re.match(r'};', line):
                    if not re.match(r'.*Members.*', line):
                        members.append(line)
                    line = next(f)
                line = next(f)
                
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

    out3 = list()
    for line in out2:
        # grib_accessor_class_validity_time_t(const char* name) : grib_accessor_class_long_t(name) {}
        print(line)
        m = re.match(r'(?P<shift>.*)grib_accessor_class_(?P<aname>[a-zA-Z0-9_]*)_t\(.*\)\s*:\s*grib_accessor_class_(?P<baseclass>[a-zA-Z0-9_]*)_t.*', line)
        # m = re.match(r'.*grib_accessor_class_validity_time_t(const char* name) : grib_accessor_class_long_t(name) {}.*', line)
        if m:
            print("XXXX");
            line = f"{m.group('shift')}grib_accessor_{m.group('aname')}_t() : grib_accessor_{m.group('baseclass')}_t(){{class_name = \"{class_name}\";}}\n"

        # replace grib_accessor_class_ with grib_accessor_
        if not re.match(r'.*include.*', line):
            line = re.sub(r'grib_accessor_class_', 'grib_accessor_', line)
            line = re.sub('\(\s*grib_accessor\s*\*\s*,\s*', '(', line) # replace (grib_accessor*, with (
            line = re.sub('\(\s*grib_accessor\s*\*\s*\)', '()', line) # replace (grib_accessor*) with ()
            line = re.sub('\(\s*grib_accessor\s*\*\s*\w*\s*,\s*', '(', line) # replace (grib_accessor* name, with (
            line = re.sub('\(\s*grib_accessor\s*\*\s*\w*\s*\)', '()', line) # replace (grib_accessor* name) with ()
            line = re.sub('int get_native_type', 'long get_native_type', line)


        out3.append(line)
    return out3


for fn in sys.argv[1:]:
    print("Processing", fn)
    out = convert(fn)
    print(''.join(out))
    out_fn = fn.split('/')[-1]
    with open(out_fn, 'w') as f:
        print("Writing to", out_fn)
        f.write(''.join(out))
