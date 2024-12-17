#!/usr/bin/env python3

import regex as re
import sys

licence_txt = '''
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
'''


virtual_func_names = ['create_empty_accessor', 'make_clone', 'next', 'ub_section', 'clear', 'compare', 'get_native_type', 'is_missing', 'nearest_smaller_value', 'notify_change', 'pack_bytes', 'pack_double', 'pack_expression', 'pack_float', 'pack_long', 'pack_missing', 'pack_string', 'pack_string_array', 'unpack_bytes', 'unpack_double', 'unpack_double_element', 'unpack_double_element_set', 'unpack_double_subarray', 'unpack_float', 'unpack_float_element', 'unpack_float_element_set', 'unpack_long', 'unpack_string', 'unpack_string_array', 'value_count', 'byte_count', 'byte_offset', 'next_offset', 'preferred_size', 'string_length', 'destroy', 'dump', 'init', 'post_init', 'resize', 'update_size']


class Transform:
    def __init__(self, file_path):
        self.file_path = file_path
        accessor_class_name = file_path.split('/')[-1].split('.')[0]
        self.name = accessor_class_name[20:]
        self.accessor_class = ""
        self.super_name = ""
        self.signatures = []

        self.scan_file()
       
    def get_name(self):
        return self.name

    def scan_file(self):
       with open(self.file_path, 'r') as f:
            for line in f:
                if re.match(r'\/\* END_CLASS_IMP \*\/', line): 
                    break

                signature = ''

                m = re.match(r'.*SUPER\s*=\s*(\w*).*', line)
                if m:
                    super_class = m.group(1)
                    self.super_name = super_class[20:]

                m = re.match(r'static (\w*) (\w*)\(\);', line)
                if m:
                    signature = f"{m.group(1)}" + ' ' + m.group(2) + '() override;'
                    self.signatures.append(signature)
                m = re.match(r'static (\w*) (\w*)\((.*)\);', line)

                if m:
                    signature = f"{m.group(1)}" + ' ' + m.group(2) + '(' + m.group(3) + ') override;'
                    self.signatures.append(signature)

                m = re.match(r'typedef struct (\w*)', line)
                if m:
                    self.accessor_class = f"class grib_accessor_{self.name}_t : public grib_accessor_{self.super_name}_t\n"
                    self.accessor_class += "{\n"
                    self.accessor_class += f"public:\n"

                    while re.match(fr'.*Members defined in {self.name}*', line) is None:
                        line = next(f)

                    while re.match(fr'.*\}}\s*grib_accessor_{self.name}\s*;\s*\n', line) is None:
                        self.accessor_class += line
                        line = next(f)
                    self.accessor_class += '};'


    def make_source(self):
        output = ""
        output += licence_txt
        output += '\n'
        output += f'#include "grib_api_internal.h"\n'
        output += f"#include \"grib_accessor_class_{self.name}.h\"\n"
        output += f'\n'
        output += f'grib_accessor_class_{self.name}_t _grib_accessor_class_{self.name}{{"{self.name}"}};\n'
        output += f'grib_accessor_class* grib_accessor_class_{self.name} = &_grib_accessor_class_{self.name};\n'
        output += f'\n'

        with open(self.file_path, 'r') as f:
            while re.match(r'\/\* END_CLASS_IMP \*\/', next(f)) is None:
                pass
            for line in f:

                m = re.match(r'static (?P<return_type>\w+) (?P<function_name>\w+)\((?P<parameters>.+)\);', line)
                if m:
                    output += f"{m.group('return_type')} {m.group('function_name')}({m.group('parameters')});"
                    continue

                m = re.match(r'static (?P<return_type>\w+) (?P<function_name>\w+)\((?P<parameters>.+)\)', line)
                if m:
                    if m.group('function_name') in virtual_func_names:
                        output += f"{m.group('return_type')} grib_accessor_class_{self.name}_t::{m.group('function_name')}({m.group('parameters')})"
                    else:
                        output += f"{m.group('return_type')} {m.group('function_name')}({m.group('parameters')})"
                    continue
                m = re.match(r'(?P<before>(.*)\s*=\s*)grib_(?P<fname>\w*)\((?P<name>[\w\->]*)\s*[,]*\s*(?P<params>.*)\);', line)
                if m and m.group('fname') in virtual_func_names:
                    output += f"{m.group('before')}{m.group('name')}->{m.group('fname')}({m.group('params')});"
                    continue
                m = re.match(r'(?P<before>.*\s*)grib_(?P<fname>\w*)\((?P<name>[\w\->]*)\s*[,]*\s*(?P<params>.*)\);', line)
                if m and m.group('fname') in virtual_func_names:
                    output += f"{m.group('before')}{m.group('name')}->{m.group('fname')}({m.group('params')});"
                    continue

                # grib_accessor_getenv* self = (grib_accessor_getenv*)a;
                m = re.match(fr'.*grib_accessor_{self.name}.*', line)
                if m:
                    output += line.replace(f'grib_accessor_{self.name}', f'grib_accessor_{self.name}_t')
                    continue

                output += line

        return output
                

    def make_header(self):
        output = ""
        output += licence_txt
        output += '\n'
        # output += '#pragma once\n'
        output += f'#ifndef eccodes_accessor_{self.name}_h\n'
        output += f'#define eccodes_accessor_{self.name}_h\n'
        output += '\n'
        output += '#include "../grib_api_internal.h"\n'
        output += f'#include "grib_accessor_class_{self.super_name}.h"\n'
        output += '\n'
        output += self.accessor_class
        output += '\n'
        output += '\n'
        output += f"class grib_accessor_class_{self.name}_t : public grib_accessor_class_{self.super_name}_t\n{{\n"
        output += f"public:\n"
        output += f"    grib_accessor_class_{self.name}_t(const char* name) : grib_accessor_class_{self.super_name}_t(name) {{}}\n"
        output += f"    grib_accessor* create_empty_accessor() override {{ return new grib_accessor_{self.name}_t{{}}; }}\n"
        for s in self.signatures:
            output += f"    {s}\n"
        output += "};\n"
        output += f"#endif /* eccodes_accessor_{self.name}_h */\n"

        return output



if __name__ == '__main__':
    args = sys.argv[1:]
    file_path = args[0]
    t = Transform(file_path)
    header = t.make_header()
    source = t.make_source()

    header_fn = f"grib_accessor_class_{t.get_name()}.h"
    source_fn = f"grib_accessor_class_{t.get_name()}.cc"
    
    with open(header_fn, 'w') as f:
        f.write(header)
    # print(header)

    with open(source_fn, 'w') as f:
        f.write(source)
    # print(source)
