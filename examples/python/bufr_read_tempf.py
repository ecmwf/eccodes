#
# Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
# Python implementation: bufr_read_tempf
#
#
# Description: read and print radiosonde data from TEMP BUFR messages.
# If available this version also lists the position information from the WMO list
# (now OSCAR/Surface) appended to the reports by ECMWF
#
# Author: Bruce Ingleby
#
# Please note that TEMP reports can be encoded in various ways in BUFR.
# Therefore the code below might not work directly for other types of TEMP
# messages than the one used in the example. It is advised to use bufr_dump to
# understand the structure of the messages.
#
from __future__ import print_function
import traceback
import sys
import numpy as np
from eccodes import *

INPUT = "../../data/bufr/PraticaTemp.bufr"
VERBOSE = 1  # verbose error reporting


def example():
    # open BUFR file
    f = open(INPUT, "rb")
    llstdonly = 1
    cnt = 0
    # loop over the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break
        cnt += 1

        # desc = codes_get_array(bufr, 'unexpandedDescriptors')
        # if all(desc != 309056):    # descent reports
        #     codes_release(bufr)
        #     continue   # Skip other templates

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data section
        codes_set(bufr, "unpack", 1)
        # get header information from the message
        try:
            sid = codes_get(bufr, "aircraftRegistrationNumberOrOtherIdentification")
        except Exception:
            sid = "UNKNOWN"

        statid = "00000   "
        try:
            block = codes_get(bufr, "blockNumber")
            stnum = codes_get(bufr, "stationNumber")
            if (block > 0) and (block < 100):  # or block != CODES_MISSING_LONG
                statid = str.format("%.2i%.3i   " % (block, stnum))
        except Exception:
            statid = "00000   "
        if statid == "00000   ":
            statid = sid[0:8]

        # subtype = codes_get(bufr,'rdbSubtype')
        sondetype = codes_get(bufr, "radiosondeType")
        slat = codes_get_array(bufr, "latitude")
        slon = codes_get_array(bufr, "longitude")
        try:
            htg = codes_get(bufr, "heightOfStationGroundAboveMeanSeaLevel")
        except Exception:
            htg = -999.0
        try:
            htp = codes_get(bufr, "heightOfBarometerAboveMeanSeaLevel")
        except Exception:
            htp = -999.0
        year = codes_get(bufr, "year")
        month = codes_get(bufr, "month")
        day = codes_get(bufr, "day")
        hour = codes_get(bufr, "hour")
        minute = codes_get(bufr, "minute")
        try:
            second = codes_get(bufr, "second")
        except Exception:
            second = 0.0
        date = str.format("%i%.2i%.2i" % (year, month, day))
        time = str.format("%.2i%.2i%.2i" % (hour, minute, second))
        try:
            windsp = codes_get_array(bufr, "windSpeed")
        except Exception:
            codes_release(bufr)
            continue
        print(
            "Ob: %7i %s %s %s %7.3f %8.3f %6.1f %6.1f %3i %4i"
            % (
                cnt,
                statid,
                date,
                time,
                slat[0],
                slon[0],
                htg,
                htp,
                sondetype,
                len(windsp),
            )
        )

        try:
            rsnumber = codes_get(bufr, "radiosondeSerialNumber")
            rssoftware = codes_get(bufr, "softwareVersionNumber")
            balloonwt = codes_get(bufr, "weightOfBalloon")
            print("RS number/software/balloonwt ", rsnumber, rssoftware, balloonwt)
        except Exception:
            rsnumber = 0
        try:
            htec = codes_get(
                bufr, "heightOfStation"
            )  # Height from WMO list (appended by ECMWF)
            print("WMO list lat, lon, ht: %7.3f %8.3f %6.1f" % (slat[1], slon[1], htec))
        except Exception:
            htec = 0

        # get all the timePeriods
        dtime = codes_get_array(bufr, "timePeriod")
        try:
            pressure = codes_get_array(bufr, "pressure")
        except Exception:
            codes_release(bufr)
            continue
        vsSignif = codes_get_array(bufr, "extendedVerticalSoundingSignificance")
        try:
            geopoth = codes_get_array(bufr, "nonCoordinateGeopotentialHeight")
        except Exception:
            codes_release(bufr)
            continue
        dlat = codes_get_array(bufr, "latitudeDisplacement")
        dlon = codes_get_array(bufr, "longitudeDisplacement")
        airt = codes_get_array(bufr, "airTemperature")
        dewt = codes_get_array(bufr, "dewpointTemperature")
        windd = codes_get_array(bufr, "windDirection")
        dtime = np.where(dtime != CODES_MISSING_LONG, dtime, np.nan)
        dlat = np.where(dlat != CODES_MISSING_DOUBLE, dlat, np.nan)
        dlon = np.where(dlon != CODES_MISSING_DOUBLE, dlon, np.nan)
        airt = np.where(airt != CODES_MISSING_DOUBLE, airt, np.nan)
        dewt = np.where(dewt != CODES_MISSING_DOUBLE, dewt, np.nan)
        windd = np.where(windd != CODES_MISSING_LONG, windd, np.nan)
        windsp = np.where(windsp != CODES_MISSING_DOUBLE, windsp, np.nan)
        geopoth = np.where(geopoth != CODES_MISSING_DOUBLE, geopoth, np.nan)
        pressure = np.where(pressure != CODES_MISSING_DOUBLE, pressure, np.nan)
        # pressure = np.where(pressure > -1e10, pressure, np.nan)
        print(
            "level  dtime   dlat   dlon pressure geopotH airTemp  dewPtT windDir  windSp  signif"
        )
        for i in range(0, len(windsp)):
            if (not llstdonly) or vsSignif[i] != 65536:
                continue
            print(
                "%5i %6.1f %6.3f %6.3f %8.1f %7.1f %7.2f %7.2f %7.2f %7.2f %7i"
                % (
                    i + 1,
                    dtime[i],
                    dlat[i],
                    dlon[i],
                    pressure[i],
                    geopoth[i],
                    airt[i],
                    dewt[i],
                    windd[i],
                    windsp[i],
                    vsSignif[i],
                )
            )
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
            sys.stderr.write(err.msg + "\n")
        return 1


if __name__ == "__main__":
    sys.exit(main())
