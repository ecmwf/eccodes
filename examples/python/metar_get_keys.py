# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

#
# Python implementation: metar_get_keys
#
# Description: how to read values of different type of keys from METAR messages
#
#

from __future__ import print_function
import traceback
import sys

from eccodes import *

INPUT = '../../data/metar/metar.txt'
VERBOSE = 1  # verbose error reporting

def print_keys(msg_id):
    keys = ['CCCC', 'latitude', 'longitude', 'dateTime',
                'elevation', 'temperature', 'dewPointTemperature', 'qnh']
    for key in keys:
        try:
            if codes_is_defined(msg_id, key):
                print('  %s: %s' % (key, codes_get(msg_id, key)))
        except CodesInternalError as err:
            print('Error with key="%s" : %s' % (key, err.msg))


def example1():
    # open METAR file
    f = open(INPUT)

    cnt = 0

    # loop for the messages in the file
    while 1:
        # get handle for message
        msg_id = codes_metar_new_from_file(f)
        if msg_id is None:
            break

        print("message: %s" % cnt)
        print_keys(msg_id)
        cnt += 1

        # delete handle
        codes_release(msg_id)

    # close the file
    f.close()


def example2():
    # This time read from a string rather than a file.
    metar_str = 'METAR LQMO 022350Z 09003KT 6000 FEW010 SCT035 BKN060 08/08 Q1003='

    # get handle for message
    msg_id = codes_new_from_message(metar_str)

    print("\nFrom string: '%s'" % metar_str)
    print_keys(msg_id)

    codes_release(msg_id)


def main():
    try:
        example1()
        example2()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1


if __name__ == "__main__":
    sys.exit(main())
