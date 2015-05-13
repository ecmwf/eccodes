#!/usr/bin/env python

import sys
from gribapi import *
import random
import traceback

VERBOSE=1
WRITE=0

class Usage(Exception):
    def __init__(self):
        pass

def test():
    # test new from sample
    #grib_release(grib_new_from_samples("GRIB2"))

    if len(sys.argv) < 2:
        raise Usage

    infile = sys.argv[1]
    outfile = infile + ".out"
    multifile = infile + ".multi"
    clonefile = infile + ".clone"
    binstrfile = infile + ".binstr"

    fid = open(infile,"r")
    while 1:
        gid = grib_new_from_file(fid)
        if not gid: break
        grib_release(gid)
    fid.close()

    fid = open(infile,"r")
    out = open(outfile,"w")
    multi = open(outfile,"w")
    clone_fid = open(clonefile,"w")
    binstr_fid = open(binstrfile,"w")

    #print "Writing from binary string to ",binstrfile
    #x = grib_read_file(fid)
    #print len(x)
    #grib_write_file(binstr_fid,x)
    #grib_close_file(binstr_fid)

    print "Operating on file '%s'" % infile

    n = grib_count_in_file(fid)
    print "Message count ",n

    # multi support test
    grib_multi_support_on()
    ismulti = "no" if (n == grib_count_in_file(fid)) else "yes"
    print "Is multi field - %s" % ismulti
    grib_multi_support_off()
    # end multi support test

    # gribex support test on/off
    print "Gribex support on/off"
    grib_gribex_mode_on()
    grib_gribex_mode_off
    # end gribex support test

    print "Browsing through messages "
    for i in range(n):
        gid = grib_new_from_file(fid)

        #grib_dump(gid)
        #grib_print(gid,"centre")

        if i == 0:
            print "Message size: ",grib_get_message_size(gid)
            nval = grib_get_size(gid,"values")
            print "Number of values in message %d is %d" % (i,nval)

            print "== %s %s %s %d ==" % \
                ( \
                grib_get_string(gid,"shortName"), \
                grib_get_string(gid,"name"), \
                grib_get_string(gid,"typeOfLevel"), \
                grib_get_long(gid,"level"), \
                )

            print "Nearest point to 10,10: "
            print grib_find_nearest(gid,10,10)

            print grib_find_nearest(gid,10,10,npoints=4)

            rand_list = []
            for i in range(0,5):
                rand_index = random.randint(1,nval)
                rand_list.append(rand_index)
                myval = grib_get_double_element(gid,"values",rand_index)
                print "Random index value[%d] = %.8f" % (rand_index,myval)

            all4rand = grib_get_double_elements(gid,"values",rand_list)
            print "All at once index values: ",all4rand

            centre = grib_get_string(gid,"centre")
            grib_set_string(gid,"centre","ecmf")
            new_centre = grib_get_string(gid,"centre")
            print "Before/after string centre: %s/%s" % (centre,new_centre)

            centre = grib_get_long(gid,"centre")
            grib_set_long(gid,"centre",99)
            new_centre = grib_get_long(gid,"centre")
            print "Before/after numeric centre: %d/%d" % (centre,new_centre)

            centre = grib_get_double(gid,"centre")
            grib_set_double(gid,"centre",9)
            new_centre = grib_get_double(gid,"centre")
            print "Before/after numeric floating point centre: %f/%f" % (centre,new_centre)

            vals = grib_get_double_array(gid,"values")
            print "Values before: ",vals[:10]
            grib_set_double_array(gid,"values",(1.0, 2.0, 3.14))
            vals = grib_get_double_array(gid,"values")
            print "Values after: ",vals[:10]

            print "Saving modified message to %s" % outfile
            if WRITE: grib_write(gid,out)

            print "Creating and saving a clone to %s" % clonefile
            clone_gid = grib_clone(gid)
            if WRITE: grib_write(clone_gid,clone_fid)
            grib_release(clone_gid)

            Ni = grib_get(gid,"Ni")
            print "Setting Ni to missing from --> ",Ni
            grib_set_missing(gid,"Ni")
            assert grib_is_missing(gid,"Ni")
            miss_Ni = grib_get(gid,"Ni")
            print "Ni is now --> ",miss_Ni
            grib_set(gid,"Ni",Ni)
            new_Ni = grib_get(gid,"Ni")
            print "Set Ni back to its original value --> ",new_Ni
            assert Ni == new_Ni

            #grib_multi_write(gid,multi)


        grib_release(gid)

    fid.close()
    out.close()
    clone_fid.close()
    print "Closed file"


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
    print "------------------------------------"
