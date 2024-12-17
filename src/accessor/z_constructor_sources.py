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
    fn_h = fn.replace('.cc', '.h')
    class_name = None
    base_name = None
    derived_name = None

    with open(fn_h, 'r') as f_h:
        members = [];
        for line in f_h:
            # match: starts with "//"
            if re.match(r'\s*//', line):
                continue

            # match: class grib_accessor_g2lon_t : public grib_accessor_double_t
            m = re.match(r'\s*class\s+grib_accessor_(?P<derived_name>\w+)_t\s*:\s*public\s+grib_accessor_(?P<base_name>\w+)_t.*', line)
            if m:
                derived_name = m.group('derived_name')
                base_name = m.group('base_name')
                continue

            # match: grib_accessor_gen_t() { class_name_ = "to_string"; }
            m = re.match(r'.*grib_accessor_(?P<base_name>\w*)_t\(\)\s*{\s*class_name_\s*=\s*(?P<class_name>".*")\s*;.*', line)
            if m:
                # members.append({'type': 'std::string', 'name': 'class_name_', 'value': m.group('class_name')})
                class_name = m.group('class_name')
                continue
            # match: const char* key_;
            m = re.match(r'\s*(?P<type>.*\*)\s+(?P<member>\w+)_;\s*', line)
            if m:
                members.append({'type': m.group('type'), 'name': m.group('member') + '_', 'value': 'nullptr'})
                continue
            # match: long
            m = re.match(r'\s*(?P<type>.*)\s+(?P<member>\w+)_;\s*', line)
            if m:
                members.append({'type': m.group('type'), 'name': m.group('member') + '_', 'value': '0'})
                continue

        for member in members:
            print(f"{member['type']} {member['name']} = {member['value']};")

    assert(class_name)
    assert(base_name)
    assert(derived_name)


    args = None
    init_body = []
    with open(fn, 'r') as f:
        # skip until init
        try:
            m = None
            line = next(f)
            while not m:
                # match: void grib_accessor_g2latlon_t::init(const long l, grib_arguments* c)
                m = re.match(r'.*grib_accessor_(?P<aname>\w+)_t::init\((?P<args>.*)\).*', line)
                if m:
                    args = m.group('args')
                line = next(f)
            while not re.match(r'{', line):
                line = next(f)
            line = next(f)
            while not re.match(r'}', line):
                # match: grib_accessor_double_t::init(l, c);
                m = re.match(r'\s*grib_accessor_(?P<base_name>\w+)_t::init\((?P<args>.*)\);.*', line)
                if not m:
                    init_body.append(line)
                line = next(f)
        except StopIteration:
            pass

    if not args:
        args = 'const long len, grib_arguments* args'

    # parse args
    print(args)
    m = re.match(r'.*const\s+long\s+(?P<arg1>\w+),\s*grib_arguments\*\s+(?P<arg2>\w+).*', args)
    args_len = m.group('arg1')
    assert(args_len)
    args_args = m.group('arg2')
    assert(args_args)


    constructor1 = []
    constructor1.append(f"grib_accessor_{derived_name}_t::grib_accessor_{derived_name}_t() :\n    grib_accessor_{base_name}_t{{}}")
    if members:
        constructor1.append(",\n")
    members_list = [f"    {member['name']}{{{member['value']}}}" for member in members]
    constructor1.append(",\n".join(members_list))
    constructor1.append("{}\n\n")
    # constructor1.append("\n")
    # constructor1.append("{\n")
    # constructor1.append(f'    class_name_ = {class_name};\n')
    # constructor1.append("}\n")

    constructor2 = []
    constructor2.append(f"grib_accessor_{derived_name}_t::grib_accessor_{derived_name}_t(const char* class_name, grib_section* section, grib_action* creator, {args}) :\n    grib_accessor_{base_name}_t{{class_name, section, creator, {args_len}, {args_args}}}")
    if members:
        constructor2.append(",\n")
    members_list = [f"    {member['name']}{{{member['value']}}}" for member in members]
    constructor2.append(",\n".join(members_list))
    constructor2.append("\n")
    constructor2.append("{\n")
    # constructor2.append(f'    class_name_ = {class_name};\n')
    constructor2.append(''.join(init_body))
    constructor2.append("}\n")


    out = []
    with open(fn, 'r') as f:
        for line in f:
            # m = re.match(r'grib_accessor\*\s+grib_accessor_(?P<aname>\w+)\s*=\s*&_grib_accessor_(?P<aname3>\w+)(?P<after>.*)', line)
            # if m:
            #     out.append(line)
            #     out.append(''.join(constructor1))
            #     out.append(''.join(constructor2))
            #     continue

            # match: void grib_accessor_bits_per_value_t::init(const long l, grib_arguments* args)
                   # void grib_accessor_g2latlon_t::init(const long l, grib_arguments* c)
            m = re.match(r'(?P<rtype>\w+)\s+grib_accessor_(?P<aname>\w+)_t::init\((?P<args>.*)\)(?P<after>.*)', line)
            if m:
                while not re.match(r'{', line):
                    line = next(f)
                while not re.match(r'}', line):
                    line = next(f)
                continue
            out.append(line)

    out.append(''.join(constructor1))
    out.append(''.join(constructor2))

    return out



for fn in sys.argv[1:]:
    print("Processing", fn)
    out = convert(fn)
    print(''.join(out))

    out_fn = fn.split('/')[-1]
    with open(out_fn, 'w') as f:
        f.write(''.join(out))
