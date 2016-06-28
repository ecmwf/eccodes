#
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
# Python implementation: bufr_read_temp
#
# Description: how to read levels from TEMP BUFR messages.
#
#
#
# Please note that TEMP reports can be encoded in various ways in BUFR.
# Therefore the code below might not work directly for other types of TEMP
# messages than the one used in the example. It is advised to use bufr_dump to
# understand the structure of the messages.
#
import traceback
import sys
from eccodes import *

INPUT = '../../data/bufr/PraticaTemp.bufr'
VERBOSE = 1  # verbose error reporting

def example():
    # open bufr file
    f = open(INPUT)
    cnt = 0
    # loop over the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break
        print "message: %s" % cnt
        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data section
        codes_set(bufr, 'unpack', 1)
        # get all the timePeriods
        timePeriod = codes_get_array(bufr, "timePeriod")
        pressure = codes_get_array(bufr, "pressure")
        extendedVerticalSoundingSignificance = codes_get_array(bufr, "extendedVerticalSoundingSignificance")
        geopotentialHeight = codes_get_array(bufr, "nonCoordinateGeopotentialHeight")
        latitudeDisplacement = codes_get_array(bufr, "latitudeDisplacement")
        longitudeDisplacement = codes_get_array(bufr, "longitudeDisplacement")
        airTemperature = codes_get_array(bufr, "airTemperature")
        dewpointTemperature = codes_get_array(bufr, "dewpointTemperature")
        windDirection = codes_get_array(bufr, "windDirection")
        windSpeed = codes_get_array(bufr, "windSpeed")
        blockNumber = codes_get(bufr, "blockNumber")
        stationNumber = codes_get(bufr, "stationNumber")
        print 'station %d%d' % (blockNumber,stationNumber)
        print 'timePeriod pressure geopotentialHeight latitudeDisplacement longitudeDisplacement airTemperature windDirection windSpeed significance'
        for i in range(0,len(windSpeed)-1):
            print timePeriod[i],pressure[i],geopotentialHeight[i],latitudeDisplacement[i],longitudeDisplacement[i],airTemperature[i],windDirection[i],windSpeed[i],extendedVerticalSoundingSignificance[i]
        cnt += 1
        # delete handle
        codes_release(bufr)
    # close the file
    f.close()

def main():
    try:
        example()
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')
        return 1
if __name__ == "__main__":
    sys.exit(main())

