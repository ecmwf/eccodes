# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

#
# Python implementation: bufr_read_temp 
#
# Description: how to read a temperature significant levels from TEMP BUFR messages.
#
#

import traceback
import sys

from eccodes import *

INPUT='../../data/bufr/temp_101.bufr'
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
        codes_set(gid,'unpack',1);
    
        # In what follows we rely on the fact that for 
        # temperature signifcant levels the value of key 
        # verticalSoundingSignificance is 4 (see flag table 8001 for details).
       
        # We also make use of the fact that in our BUFR message
        # verticalSoundingSignificance is always followed by geopotential, 
        # airTemperature, dewpointTemperature,
        # windDirection, windSpeed and pressure. 
        
        #-------------------------------------------------------
        # Get the number of the temperature significant levels.
        #------------------------------------------------------

        # We find out the number of significant temperature levels by 
        # counting how many presssure values we have on these levels. 
        
        numSigT=codes_get_size(gid,"/verticalSoundingSignificance=4/pressure")
        print  '  Number of T significant levels %ld' % (numSigT)
         
        # ----------------------------
        # Get pressure 
        # ----------------------------

        sigt_pres=codes_get_array(gid,"/verticalSoundingSignificance=4/pressure")
        
        #--------------------------------
        # Get gepotential
        #--------------------------------

        sigt_geo=codes_get_array(gid,"/verticalSoundingSignificance=4/geopotential")

        if len(sigt_geo) != sigTNum :
            print "incosistent number of geopotential values found!"
            return 1
        
        #--------------------------------
        # Get temperature
        #--------------------------------

        sigt_t=codes_get_array(gid,"/verticalSoundingSignificance=4/airTemperature")

        if len(sigt_t) != sigTNum :
            print "incosistent number of temeprature values found!"
            return 1
        
        #--------------------------------
        # Get dew point
        #--------------------------------

        sigt_td=codes_get_array(gid,"/verticalSoundingSignificance=4/dewpointTemperature")

        if len(sigt_td) != sigTNum :
            print "incosistent number of dewpoint temperature values found!"
            return 1
        
        #------------------------------------
        # Print the values
        # -----------------------------------
    
        print "lev  pres    geo    t    td"
        print "-------------------------------"
    
        for i in xrange(sigTNum):
            print "%3d %6.0f %6.0f %.1f %.1f" % (i+1,sigt_pres[i],sigt_geo[i],sigt_t[i],sigt_td[i])

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
