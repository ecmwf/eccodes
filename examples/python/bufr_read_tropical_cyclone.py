# Copyright 2005-2015 ECMWF.
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
# Please note that scatterometer data can be encoded in various ways in BUFR. Therefore the code
# below might not work directly for other types of messages than the one used in the
# example. It is advised to use bufr_dump first to understand the structure of these messages.
#


import traceback
import sys

from eccodes import *

INPUT='../../data/bufr/tropical_cyclone.bufr'
VERBOSE=1 # verbose error reporting

def example():

    # open bufr file
    f = open(INPUT)

    cnt=0

    # loop for the messages in the file
    while 1:
        # get handle for message
        gid = codes_bufr_new_from_file(f)
        if gid is None: break

        print "message: %s" % cnt

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        codes_set(gid,'unpack',1)

# The BUFR file contains a single message with number of subsets in a compressed form.         
# It means each subset has exactly the same structure.         
# One subset contains values of latitude, longitude, pressure, wind at 10m          
# for one particular ensemble member over forecast period from 0h to 240h by 6h step.          
# To print values of latitude, longitude, pressure, wind at 10m for particular ensemble member from all the subsets         
# we will simply access the key by condition (see below)        
#  
# The key latitude will give back the array of all the values corresponding 
#to all the instances of the key in the tree. 
# The key #2#latitude will return only the values of the second instance in the tree.        

        # Get the total number of subsets.
        numObs=codes_get(gid,"numberOfSubsets")
        
        #print '  Number of values: %ld'  % (numObs)
        
        year=codes_get(gid,"year")
        month=codes_get(gid,"month")
        day=codes_get(gid,"day")

        # Get ensembleMemberNumber
        ensembleMemberNumber= codes_get_array(gid,"ensembleMemberNumber")

        #Get the time step
        nTimeSteps=len(codes_get_array(gid,"timePeriod")) 
        ts_list=codes_get_array(gid,"timePeriod")
        for rank in xrange(nTimeSteps):
           rank1=(rank+1)*2+2
           rank3=(rank+1)*2+3

           #print "time step %d"%ts_list[rank]

           #Get the time step
           timeStep=codes_get_array(gid,"#%d#timePeriod"%(rank+1))
        
           # Get Meteorological Attribute Significance
           metAttSignificance=codes_get_array(gid,"#%d#meteorologicalAttributeSignificance"%rank1)

           #Get latitude (for all the subsets)
           lat=codes_get_array(gid,"#%d#latitude"%rank1)
        
           #Get longitude (for all the subsets)
           lon=codes_get_array(gid,"#%d#longitude"%rank1)
         
           #Get  pressureReducedToMeanSeaLevel(for all the subsets)
           press=codes_get_array(gid,"#%d#pressureReducedToMeanSeaLevel"%(rank+2)) 
           
           # Get Meteorological Attribute Significance
           metAttSignificance3=codes_get_array(gid,"#5#meteorologicalAttributeSignificance")

           #Get latitude (for all the subsets)
           lat3=codes_get_array(gid,"#%d#latitude"%rank3)
        
           #Get longitude (for all the subsets)
           lon3=codes_get_array(gid,"#%d#longitude"%rank3)

           #Get  windSpeedAt10M(for all the subsets)
           wind3=codes_get_array(gid,"#%d#windSpeedAt10M"%(rank+2));
           
           # Check that all arrays are same size
           if len(lat) != numObs or len(lon) != numObs or len(press) != numObs :
               print 'inconsistent array dimension'
               return 1
           
           print 'Date  %d %d %d   Time step %d'%(day, month,year,ts_list[rank])
           print " metAttSignificance %d %s metAttSignificance %d"%(metAttSignificance,"  ", metAttSignificance3) 
           print "ens  lat  lon  press        lat  lon  wind10m"
           for i in xrange(numObs):
               if lat[i]!=CODES_MISSING_DOUBLE:
                   print "%s %.6g %s %.6g %.6g %.6g %s %.6g %.6g %.6g" %("",ensembleMemberNumber[i]," ",lat[i],lon[i],press[i],"     ",lat3[i],lon3[i],wind3[i])

        cnt+=1

        # delete handle
        codes_release(gid)

    # close the file
    f.close()

def main():
    try:
        example()
    except CodesInternalError,err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr,err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())
