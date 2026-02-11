#!/usr/bin/env python3

#open a file

import sys
import re

f = open('eccodes_backup.cc', 'r')
f_out = open('eccodes.cc', 'w')
f_out.write('#include "ExceptionHandler.h"')

# read the file line by line
for line in f:



    # match: int codes_count_in_filename(codes_context* c, const char* filename, int* n)
    # match: void codes_print_api_version(FILE* out)
    # m = re.match(r'[a-z0-9\*]*\s*[a-z0-9\*]*\s+(?P<name>\w*)\((?P<params>.*)\)', line.strip())
    m = re.match(r'(?P<rettype>\w*\s*[\w\*]+)\s+(?P<funcname>[\w_]*)\((?P<params>[\w\*,\s]*)\)$', line.strip())
    orig_line = line.strip()
    if m:
        m3 = re.match(r'err', line.strip())
        has_error = False
        if m3:
            has_error = True
            continue

        handle_error = None
        if m.group('rettype') == 'void':
            if has_error:
                handle_error = f'return eccodes::logErrorAndReturnValue(result, err);'
            else:
                handle_error = f'return eccodes::logErrorAndReturnValue(result);'
        elif m.group('rettype') == 'int':
            if has_error:
                handle_error = f'return eccodes::updateErrorAndReturnValue(result, err)'
            else:
                handle_error = f'return eccodes::selectError(result)'
        else:
            if has_error:
                handle_error = f'return eccodes::updateErrorAndReturnValue(result, err)'
            else:
                handle_error = f'return eccodes::selectError(result)'
        f_out.write(line)
        print(line.strip())
        # print(line.strip(), m.group('rettype'), m.group('funcname'), m.group('params'))
        # next line
        next_line = next(f)
        f_out.write(next_line)
        next_line = next(f)
        f_out.write(next_line)
        # print("\t", next_line.strip())

        m2_1 = re.match(r'\s*return\s+(?P<funcname>[\w_]+)\((?P<args>.*)\);', next_line.strip())
        m2_2 = re.match(r'\s*(?P<funcname>[\w_]+)\((?P<args>.*)\);', next_line.strip())
        m2 = None
        if m2_1:
            m2 = m2_1
        elif m2_2:
            m2 = m2_2
        m2_funcname = None;
        m2_args = None;
        if m2:
            print("\t", m2.group('funcname'), m2.group('args'))
            m2_funcname = m2.group('funcname')
            m2_args = m2.group('args')
        next_line = next(f)
        f_out.write(next_line)

        f_out.write(f"{m.group('rettype')} {m2_funcname}({m.group('params')})\n")
        f_out.write('{\n')
        # f_out.write(f'\t{m2_funcname}_({m.group("params")});\n')
        if handle_error is None:
            # throw an exception in Python and exit
            print(f"Bad line {orig_line}")
            quit()

        f_out.write(f'    auto result = eccodes::handleExceptions({m2_funcname}_, {m2_args});\n')
        f_out.write(f'    {handle_error};\n')
        f_out.write('}\n\n')

    else:
        f_out.write(line)


        


