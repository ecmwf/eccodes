# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
# Python implementation:  bufr_read_tropical_cyclone
#
# Description: how to read data of the ECMWF EPS tropical cyclone tracks encoded in BUFR format.
#

from __future__ import print_function
import traceback
import sys
import os
import getopt
from eccodes import *

VERBOSE = 1  # verbose error reporting
default_namespace = 'ls'


def do_print(namespace, INPUT):
    f = open(INPUT)
    first_time = True

    print('{')
    print('   "messages" : [')
    while 1:
        gid = codes_grib_new_from_file(f)
        if gid is None:
            break

        if not first_time:
            print('      ,{')
        else:
            print('      {')
            first_time = False

        iterid = codes_keys_iterator_new(gid, namespace)

        f1 = True
        while codes_keys_iterator_next(iterid):
            keyname = codes_keys_iterator_get_name(iterid)
            keyval = codes_get_string(iterid, keyname)
            if not f1:
                print(',')
            else:
                print('')
                f1 = False
            print("         \"%s\" : \"%s\"" % (keyname, keyval), end=' ')

        print('')
        print('      }')
        codes_keys_iterator_delete(iterid)
        codes_release(gid)

    print('   ]')
    print('}')
    f.close()


def usage():
    progname = os.path.basename(sys.argv[0])
    print("Usage: ", progname, "[options] grib_file1 grib_file2 ...")
    print('Options:')
    print('\t-n namespace')
    print('\t\tAll the keys belonging to namespace are printed.')
    print('\t-m Mars keys are printed.')
    print('')


def main():
    if len(sys.argv) < 2:
        usage()
        return 1
    try:
        options = "mn:"
        namespace = default_namespace
        opts, args = getopt.getopt(sys.argv[1:], options)
        for o, a in opts:
            if o == '-m':
                namespace = 'mars'
            elif o == '-n':
                namespace = a or default_namespace
            else:
                assert False, 'Invalid option'

        # Check we have some GRIB files to process
        if not args:
            usage()
            return 1
        for arg in args:
            do_print(namespace, arg)
    except getopt.GetoptError as err:
        print('Error: ', err)
        usage()
        return 1
    except GribInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print(err.msg, file=sys.stderr)

        return 1


if __name__ == "__main__":
    sys.exit(main())
