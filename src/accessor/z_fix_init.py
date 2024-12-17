#!/usr/bin/env python3

import regex as re
import sys

class Editor:
    def __init__(self,  header_path, source_path):
        self.header_path = header_path
        self.source_path = source_path

        self.prepare()

        print(f"Name: {self.name}")
        print(f"Parent Name: {self.parent_name}")
    
    def prepare(self):
        with open(self.header_path, 'r') as f:
            for line in f:
                m = re.match(r'.*class grib_accessor_class_(?P<name>\w*)_t : public grib_accessor_class_(?P<super_name>\w*)_t.*', line)
                if m:
                    self.name = m.group('name')
                    self.parent_name = m.group('super_name')

    def fix_init(self):
        with open(self.source_path, 'r') as f:
            for line in f:
                m = re.match(r'\s*grib_accessor_class_(?P<name>\w*)_t::init\(\s*\w+\s*,\s*\w+\s*,\s*\w+\s*\)', line)
                if m:
                    return None

        with open(self.source_path, 'r') as f:
            output = ""
            for line in f:
                m = re.match(r'void grib_accessor_class_(?P<name>\w*)_t::init\(grib_accessor\*\s*(?P<accessor>\w+)\s*,\s+const\s+long\s+(?P<length>\w+)\s*, grib_arguments\*\s+(?P<args>\w+)\s*\)(?P<end>.*)', line)
                if m:
                    output += line
                    if not m.group('end').strip().startswith("{"):
                        output += next(f)

                    # output += f"void grib_accessor_class_{self.name}_t::init(grib_accessor* {m.group('accessor')}, const long {m.group('length')}, grib_arguments* {m.group('args')})\n"
                    # output += "{\n"
                    output += f"    grib_accessor_class_{self.parent_name}_t::init({m.group('accessor')}, {m.group('length')}, {m.group('args')});\n"
                    continue
                else:
                    output += line
            return output

if __name__ == "__main__":
    paths= sys.argv[1:]

    black_list = [
            "grib_accessor.cc",
            "grib_accessor_class.cc",
            "grib_accessor_class_gen.cc",
            "grib_accessors_list.cc",
    ]

    for path in paths:
        print(f"Processing {path}")
        if path in black_list:
            continue
        basename = path.split('.')[0]
        header_path = f"{basename}.h"
        source_path = f"{basename}.cc"
         

        editor = Editor(header_path, source_path)
        new_source = editor.fix_init()
        
        if new_source is not None:
            # print(new_source)
            with open(source_path, 'w') as f:
                f.write(new_source)
        else:
            print("Already fixed")
