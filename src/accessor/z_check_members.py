#!/usr/bin/env python3

import sys
import re
from collections import defaultdict


class Member:
    def __init__(self, line):
        self.type = None
        self.name = None
        self.is_const = False
        self.is_pointer = False

        # match: double dval_;
        if m := re.match(r'\s*(?P<type>\w+)\s+(?P<name>\w+_);.*', line):
            self.type = m.group('type')
            self.name = m.group('name')
            self.is_const = False
            self.is_pointer = False

        # match: const double dval_;
        if m := re.match(r'\s*const\s+(?P<type>\w+)\s+(?P<name>\w+_);.*', line):
            self.type = m.group('type')
            self.name = m.group('name')
            self.is_const = True
            self.is_pointer = False

        # match: double* dval_;
        if m := re.match(r'\s*(?P<type>\w+)\*\s+(?P<name>\w+_);.*', line):
            self.type = m.group('type')
            self.name = m.group('name')
            self.is_const = False
            self.is_pointer = True

        # match: const double* dval_;
        if m := re.match(r'\s*const\s+(?P<type>\w+)\*\s+(?P<name>\w+_);.*', line):
            self.type = m.group('type')
            self.name = m.group('name')
            self.is_const = True
            self.is_pointer = True


    def __str__(self):
        if self.is_const:
            const = 'const '
        else:
            const = ''
        if self.is_pointer:
            pointer = '*'
        else:
            pointer = ''
        return f"{const}{self.type}{pointer} {self.name}"

    def is_member(self):
        return self.type is not None


class Header:
    def __init__(self, filename):
        self.filename = filename
        self.this_name = None
        self.base_name = None
        self.members = []

        for line in open(filename):
            member = Member(line)
            if member.is_member():
                self.members.append(member)
                continue

            # match: class grib_accessor_g2lon_t : public grib_accessor_double_t
            # if m := re.match(r'\s*class grib_accessor_(?P<name>[\w_]+)_t : public grib_accessor_(?P<base>[\w_]+)_t', line):
            if m := re.match(r'\s*class (?P<name>[\w_]+) : public (?P<base>[\w_]+)', line):
                self.this_name = m.group('name')
                self.base_name = m.group('base')
                continue

            # match: class grib_accessor_g2lon_t : public grib_accessor_double_t
            if m := re.match(r'\s*class (?P<name>[\w_]+) : public grib_accessor', line):
                self.this_name = m.group('name')
                self.base_name = "grib_accessor"
                continue

            # match: class grib_accessor:
            if m := re.match(r'\s*class grib_accessor$', line):
                self.this_name = "grib_accessor"
                self.base_name = None
                continue

    def as_string(self):
        out = []
        out.append(f"this_name: {self.this_name}")
        out.append(f"base_name: {self.base_name}")
        out.append("Members:")
        for member in self.members:
            out.append(f"    {member}\n")
        return out




children = defaultdict(list)
headers = dict()
for fn in sys.argv[1:]:
    print("Processing", fn)
    h = Header(fn)
    headers[h.this_name] = h
    children[h.base_name].append(h.this_name)
    # out = h.as_string()
    # print(out)

# print(headers["grib_accessor_variable_t"].members)
print(headers["grib_accessor"].members)
for name, header in headers.items():
    for member in header.members:
        print(f"{name}: {member}")


def get_children_(base_name):
    children_list = children[base_name]
    if not children_list:
        return [[base_name]]
    else:
        result = []
        for child in children_list:
            children2 = get_children_(child)
            for child2 in children2:
                result.append([base_name] + child2)
        return result


def make_hierarchy():
    hierarchy = []
    base_name = "grib_accessor"
    children_ = children[base_name]
    if not children_:
        hierarchy.append([base_name])
    else:
        for child in children_:
            chains = get_children_(child)
            for chain in chains:
                hierarchy.append([base_name] + chain)

    return hierarchy


hierarchy = make_hierarchy()
print("Hierarchy:")
for chain in hierarchy:
    print(" -> ".join(chain))
    # print(" -> ".join(chain[0:-1]))
    # for name in chain:
    #     print("    ", name)
    #     for member in headers[name].members:
    #         print("        ", member)


# grib_accessor
# grib_accessor_abstract_long_vector_t (TODO)
# grib_accessor_abstract_vector_t (TODO)
# grib_accessor_ascii_t (DONE)
# grib_accessor_bitmap_t (DONE)
# grib_accessor_bytes_t (DONE)
# grib_accessor_data_complex_packing_t (DONE)
# grib_accessor_data_g1simple_packing_t (DONE)
# grib_accessor_data_g2simple_packing_t (DONE)
# grib_accessor_data_secondary_bitmap_t (DONE)
# grib_accessor_data_shsimple_packing_t (DONE)
# grib_accessor_data_simple_packing_t (DONE)
# grib_accessor_double_t (DONE)
# grib_accessor_evaluate_t (DONE)
# grib_accessor_g1date_t (DONE)
# grib_accessor_g1step_range_t (DONE)
# grib_accessor_gen_t (DONE)
# grib_accessor_long_t (DONE)
# grib_accessor_padding_t (DONE)
# grib_accessor_section_length_t (DONE)
# grib_accessor_to_double_t (DONE)
# grib_accessor_unsigned_t (DONE)
# grib_accessor_values_t (DONE)

# sed -i -z 's/public/private/3' grib_accessor_class*.h


# sed -i 's/private/public/g' grib_accessor.h grib_accessor_class_abstract_long_vector.h grib_accessor_class_abstract_vector.h grib_accessor_class_ascii.h grib_accessor_class_bitmap.h grib_accessor_class_bytes.h grib_accessor_class_data_complex_packing.h grib_accessor_class_data_g1simple_packing.h grib_accessor_class_data_g2simple_packing.h grib_accessor_class_data_secondary_bitmap.h grib_accessor_class_data_shsimple_packing.h grib_accessor_class_data_simple_packing.h grib_accessor_class_double.h grib_accessor_class_evaluate.h grib_accessor_class_g1date.h grib_accessor_class_g1step_range.h grib_accessor_class_gen.h grib_accessor_class_long.h grib_accessor_class_padding.h grib_accessor_class_section_length.h grib_accessor_class_to_double.h grib_accessor_class_unsigned.h grib_accessor_class_values.h 
