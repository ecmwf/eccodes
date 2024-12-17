#!/usr/bin/env python3

import regex as re
# Read file line by line

virtual_func_names = ['create_empty_accessor', 'make_clone', 'next', 'sub_section', 'clear', 'compare', 'get_native_type', 'is_missing', 'nearest_smaller_value', 'notify_change', 'pack_bytes', 'pack_double', 'pack_expression', 'pack_float', 'pack_long', 'pack_missing', 'pack_string', 'pack_string_array', 'unpack_bytes', 'unpack_double', 'unpack_double_element', 'unpack_double_element_set', 'unpack_double_subarray', 'unpack_float', 'unpack_float_element', 'unpack_float_element_set', 'unpack_long', 'unpack_string', 'unpack_string_array', 'value_count', 'byte_count', 'byte_offset', 'next_offset', 'preferred_size', 'string_length', 'destroy', 'dump', 'init', 'post_init', 'resize', 'update_size']


def convert_call(line):
        # int err    = grib_unpack_string(a, val, &l);
    m = re.match(r'(?P<before>(.*)\s*=\s*)grib_(?P<fname>\w*)\((?P<name>\w*)\s*[,]*\s*(?P<params>.*)\);', line)
    if m and m.group('fname') in virtual_func_names:
        return f"{m.group('before')}{m.group('name')}->{m.group('fname')}({m.group('params')});"
    return line

signatures = dict()

class Transform:
    def __init__(self, file_path):
        self.file_path = file_path

    def read_file(self):
        accessor_class_name = self.file_path.split('/')[-1].split('.')[0]
        accessor_class = ""
        name = accessor_class_name[20:]
        super_name = ""
        with open(self.file_path, 'r') as f:
            signatures = []
            for line in f:
                signature = ''
                m = re.match(r'.*SUPER\s*=\s*(\w*).*', line)
                if m:
                    super_class = m.group(1)
                    super_name = super_class[20:]
                    print(f"name {name} supername {super_name}")
                m = re.match(r'static (\w*) (\w*)\(grib_accessor\*\);', line)
                if m:
                    signature = f"{m.group(1)}" + ' ' + m.group(2) + '() override;'
                    signatures.append(signature)
                m = re.match(r'static (\w*) (\w*)\(grib_accessor\*, (.*)\);', line)
                if m:
                    signature = f"{m.group(1)}" + ' ' + m.group(2) + '(' + m.group(3) + ') override;'
                    signatures.append(signature)

                m = re.match(r'typedef struct (\w*)', line)
                if m:
                    accessor_class = f"class grib_accessor_{name}_t : public grib_accessor_{super_name}_t\n"
                    line = next(f)
                    line = next(f)
                    accessor_class += "{\n"
                    accessor_class += f"public:"
                    while line != f'}} grib_accessor_{name};\n':
                        accessor_class += line
                        line = next(f)
                    accessor_class += '};'
                    
                m1 = re.match(r'static (?P<return_type>\w+) (?P<function_name>\w+)\((?P<parameters>.+)\)', line)
                if m1:
                    function = ""
                    function += f"{m1.group('return_type')} grib_accessor_class_{name}_t::{m1.group('function_name')}({m1.group('parameters')})"
                    line = next(f)
                    line = next(f)
                    function += "\n{\n"
                    while re.match(r'}', line) is None:
                        function += convert_call(line)
                        line = next(f)
                    function += '}\n'
                    print(f"{function}")


        print('#pragma once')
        print('#include "../grib_api_internal.h"')
        print(f'#include "grib_accessor_class_{super_name}.h"')
        print(accessor_class)
        print(f"class grib_accessor_class_{name}_t : public grib_accessor_class_{super_name}_t\n{{")
        print(f"public:")

        print(f"\tgrib_accessor_class_{name}_t(const char* name) : grib_accessor_class_{super_name}_t(name) {{}}")
        print(f"\tgrib_accessor* create_empty_accessor() override {{ return new grib_accessor_{name}_t{{}}; }}")
        for s in signatures:
            print(f"\t{s}")
        print("};")
                
            
            


if __name__ == '__main__':
    file_path = 'todo/grib_accessor_class_ascii.cc'
    t = Transform(file_path)
    t.read_file()
