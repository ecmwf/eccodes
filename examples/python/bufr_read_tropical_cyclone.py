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
import collections

from eccodes import *

INPUT = '../../data/bufr/tropical_cyclone.bufr'
VERBOSE = 1  # verbose error reporting

data = collections.defaultdict(dict)


def example():
    # open BUFR file
    f = open(INPUT)

    cnt = 0

    # loop for the messages in the file
    while 1:
        # get handle for message
        bufr = codes_bufr_new_from_file(f)
        if bufr is None:
            break

        print('**************** MESSAGE: ', cnt + 1, '  *****************')

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        codes_set(bufr, 'unpack', 1)

        numObs = codes_get(bufr, "numberOfSubsets")
        year = codes_get(bufr, "year")
        month = codes_get(bufr, "month")
        day = codes_get(bufr, "day")
        hour = codes_get(bufr, "hour")
        minute = codes_get(bufr, "minute")

        print('Date and time: ', day, '.', month, '.', year, '  ', hour, ':', minute)

        stormIdentifier = codes_get(bufr, "stormIdentifier")
        print('Storm identifier: ', stormIdentifier)

        # How many different timePeriod in the data structure?
        numberOfPeriods = 0
        while True:
            numberOfPeriods = numberOfPeriods + 1
            try:
                codes_get_array(bufr, "#%d#timePeriod" % numberOfPeriods)
            except CodesInternalError as err:
                break
                # the numberOfPeriods includes the analysis (period=0)

        # Get ensembleMemberNumber
        memberNumber = codes_get_array(bufr, "ensembleMemberNumber")
        memberNumberLen = len(memberNumber)

        # Observed Storm Centre
        significance = codes_get(bufr, '#1#meteorologicalAttributeSignificance')
        latitudeCentre = codes_get(bufr, '#1#latitude')
        longitudeCentre = codes_get(bufr, '#1#longitude')

        if significance != 1:
            print('ERROR: unexpected #1#meteorologicalAttributeSignificance')
            return 1

        if (latitudeCentre == CODES_MISSING_DOUBLE) and (longitudeCentre == CODES_MISSING_DOUBLE):
            print('Observed storm centre position missing')
        else:
            print('Observed storm centre: latitude=', latitudeCentre, ' longitude=', longitudeCentre)

        # Location of storm in perturbed analysis
        significance = codes_get(bufr, '#2#meteorologicalAttributeSignificance')

        if significance != 4:
            print('ERROR: unexpected #2#meteorologicalAttributeSignificance')
            return 1

        latitudeAnalysis = codes_get_array(bufr, '#2#latitude')
        longitudeAnalysis = codes_get_array(bufr, '#2#longitude')
        pressureAnalysis = codes_get_array(bufr, '#1#pressureReducedToMeanSeaLevel')

        # Location of Maximum Wind
        significance = codes_get(bufr, '#3#meteorologicalAttributeSignificance')

        if significance != 3:
            print('ERROR: unexpected #3#meteorologicalAttributeSignificance=', significance)
            return 1

        latitudeMaxWind0 = codes_get_array(bufr, '#3#latitude')
        longitudeMaxWind0 = codes_get_array(bufr, '#3#longitude')
        windMaxWind0 = codes_get_array(bufr, '#1#windSpeedAt10M')

        if len(latitudeAnalysis) == len(memberNumber) and len(latitudeMaxWind0) == len(memberNumber):
            for k in range(len(memberNumber)):
                data[k][0] = [latitudeAnalysis[k], longitudeAnalysis[k], pressureAnalysis[k], latitudeMaxWind0[k],
                              longitudeMaxWind0[k], windMaxWind0[k]]

        else:
            for k in range(len(memberNumber)):
                data[k][0] = [latitudeAnalysis[0], longitudeAnalysis[0], pressureAnalysis[k], latitudeMaxWind0[0],
                              longitudeMaxWind0[0], windMaxWind0[k]]

        timePeriod = [0 for x in range(numberOfPeriods)]
        for i in range(1, numberOfPeriods):
            rank1 = i * 2 + 2
            rank3 = i * 2 + 3

            ivalues = codes_get_array(bufr, "#%d#timePeriod" % i)

            if len(ivalues) == 1:
                timePeriod[i] = ivalues[0]
            else:
                for j in range(len(ivalues)):
                    if ivalues[j] != CODES_MISSING_LONG:
                        timePeriod[i] = ivalues[j]
                        break

            # Location of the storm
            values = codes_get_array(bufr, "#%d#meteorologicalAttributeSignificance" % rank1)
            if len(values) == 1:
                significance = values[0]
            else:
                for j in range(len(values)):
                    if values[j] != CODES_MISSING_LONG:
                        significance = values[j]
                        break

            if significance == 1:
                lat = codes_get_array(bufr, "#%d#latitude" % rank1)
                lon = codes_get_array(bufr, "#%d#longitude" % rank1)
                press = codes_get_array(bufr, "#%d#pressureReducedToMeanSeaLevel" % (i + 1))
            else:
                print('ERROR: unexpected meteorologicalAttributeSignificance=', significance)

            # Location of maximum wind
            values = codes_get_array(bufr, "#%d#meteorologicalAttributeSignificance" % rank3)
            if len(values) == 1:
                significanceWind = values[0]
            else:
                for j in range(len(values)):
                    if values[j] != CODES_MISSING_LONG:
                        significanceWind = values[j]
                        break

            if significanceWind == 3:
                latWind = codes_get_array(bufr, "#%d#latitude" % rank3)
                lonWind = codes_get_array(bufr, "#%d#longitude" % rank3)
                wind10m = codes_get_array(bufr, "#%d#windSpeedAt10M" % (i + 1))
            else:
                print('ERROR: unexpected meteorologicalAttributeSignificance=', significanceWind)

            for k in range(len(memberNumber)):
                data[k][i] = [lat[k], lon[k], press[k], latWind[k], lonWind[k], wind10m[k]]


            # ---------------------------------------- Print the values -------------

        for m in range(len(memberNumber)):
            print("== Member  %d" % memberNumber[m])
            print("step  latitude  longitude   pressure  latitude   longitude    wind")
            for s in range(len(timePeriod)):
                if data[m][s][0] != CODES_MISSING_DOUBLE and data[m][s][1] != CODES_MISSING_DOUBLE:
                    print(" {0:>3d}{0}{0:>6.1f}{0}{0:>6.1f}{0}{0:>8.1f}{0}{0:>6.1f}{0}{0:>6.1f}{0}{0:>6.1f}".format(
                        timePeriod[s], '  ', data[m][s][0], '     ', data[m][s][1], '     ', data[m][s][2], '  ',
                        data[m][s][3], '     ', data[m][s][4], '     ', data[m][s][5]))

                # -----------------------------------------------------------------------
        cnt += 1

        # release the BUFR message
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
