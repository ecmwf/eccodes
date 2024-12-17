#!/usr/bin/env python3

import sys
import re

def convert(fn):
    out = list()
    class_name = None

    with open(fn, 'r') as f:
        for line in f:
            #match: grib_accessor_double_t() { class_name_ = "abstract_vector"; }
            m = re.match(r'.*grib_accessor_(?P<name>\w*)_t\(\)\s*{\s*class_name_\s*=\s*"(?P<class_name>.*)"\s*;.*', line)
            if m:
                class_name = m.group('class_name')
                break

    assert(class_name)

    with open(fn, 'r') as f:
        members = list()
        for line in f:
            
            # match: grib_accessor_abstract_vector_t() :
            m = re.match(r'.*grib_accessor_(?P<derived_name>\w*)_t\(\)\s*:.*', line)
            if m:
                out.append(f"    grib_accessor_{m.group('derived_name')}_t();\n")
                out.append(f"    grib_accessor_{m.group('derived_name')}_t(const char*, grib_section*, grib_action*, const long, grib_arguments*);\n")
                continue

            #match: grib_accessor_double_t() { class_name_ = "abstract_vector"; }
            m = re.match(r'.*grib_accessor_(?P<name>\w*)_t\(\)\s*{\s*class_name_\s*=\s*"(?P<class_name>.*)"\s*;.*', line)
            if m:
                continue

            # m = re.match(r'.*grib_accesso
            # _class_[a-zA-Z0-9_]*_t\s*_grib_accessor_class_[a-zA-Z0-9_]*\s*{\s*"(?P<class_name>.*)"\s*};.*', line)
            # match grib_accessor* create_empty_accessor() override { return new grib_accessor_dictionary_t{}; }
            m = re.match(r'.*grib_accessor\*\s*create_empty_accessor\(\)\s*override\s*{\s*return\s*new\s*grib_accessor_(?P<class_name>[a-zA-Z0-9_]*)_t{};\s*}.*', line)
            if m:
                print("class_name:", m.group('class_name'))
                out.append(f"    grib_accessor* create_empty_accessor(grib_section* p, grib_action* creator, const long len, grib_arguments* args) override\n    {{\n        return new grib_accessor_{m.group('class_name')}_t{{\"{class_name}\", p, creator, len, args}};\n    }}\n")
                continue

            # match void init(const long len, grib_arguments* arg) override;
            m = re.match(r'.*void\s*init\(\s*const\s*long\s*(?P<len>\w*),\s*grib_arguments\*\s*(?P<arg>\w*)\s*\)\s*override;\s*', line)
            if m:
                continue

            # # match void destroy(grib_context* context) override;
            # m = re.match(r'.*void\s*destroy\(\s*grib_context\*\s*context\s*\)\s*override;\s*', line)
            # if m:
            #     continue
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
