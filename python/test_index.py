#!/usr/bin/env python

import sys
from gribapi import *
from array import array
import random
import traceback
import itertools

VERBOSE=1
WRITE=0

class Usage(Exception):
    def __init__(self):
        pass

def product(*args, **kwds):
    # product('ABCD', 'xy') --> Ax Ay Bx By Cx Cy Dx Dy
    # product(range(2), repeat=3) --> 000 001 010 011 100 101 110 111
    pools = map(tuple, args) * kwds.get('repeat', 1)
    result = [[]]
    for pool in pools:
        result = [x+[y] for x in result for y in pool]
    for prod in result:
        yield tuple(prod)

def test():
    # test new from sample
    #grib_release(grib_new_from_samples("GRIB2"))

    if len(sys.argv) < 2:
        raise Usage

    infile = sys.argv[1]
    index_keys = ["shortName","level","number","step"]

    print "indexing..."
    iid = grib_index_new_from_file(infile,index_keys)
    print "end indexing..."

    index_vals = []

    for key in index_keys:
        print "%sSize=%d" % (
            key,
            grib_index_get_size(iid,key)
        )

        key_vals = grib_index_get_string(iid,key)
        print " ".join(key_vals)

        index_vals.append(key_vals)

    for prod in product(*index_vals):
        for i in range(len(index_keys)):
            grib_index_select_string(iid,index_keys[i],str(prod[i]))

        while 1:
            gid = grib_new_from_index(iid)
            if gid is None: break
            print " ".join(["%s=%s" % (key,grib_get_string(gid,key)) for key in index_keys])
            grib_release(gid)

    grib_index_release(iid)


def main():
    try:
        test()
    except GribInternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr,err.msg

        return 1
    except Usage:
        print "Usage: %s infile" % sys.argv[0]
        sys.exit(2)

if __name__ == "__main__":
    main()
    #print "------------------------------------"
