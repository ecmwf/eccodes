# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

import traceback
import numpy
import sys

from eccodes import *


def example(csvfile, input_filename, output_filename):

    fbufrin = open(input_filename,  'rb')
    fbufrout = open(output_filename, 'wb')

    print 'Using ecCodes version: ', codes_get_api_version()

    # The first line in the CSV has the column names
    print 'Reading input CSV file: ', csvfile
    data = numpy.genfromtxt(csvfile, delimiter=',', dtype=None, names=True)

    ymd_column = data['ymd']
    years = map(lambda x: int(str(x)[0:4]), ymd_column)
    months = map(lambda x: int(str(x)[4:6]), ymd_column)
    days = map(lambda x: int(str(x)[6:8]), ymd_column)

    time_column = data['time']
    hours = map(lambda x: int(str(x)[0:2]), time_column)
    minutes = map(lambda x: int(str(x)[3:5]), time_column)
    seconds = map(lambda x: int(str(x)[6:8]), time_column)

    latitudes = data['latitude']
    longitudes = data['longitude']
    altitudes = data['altitude']
    pressures = data['pressure']
    windSpeeds = data['windSpeed']
    windDirections = data['windDirection']
    temperatures = data['temperature']

    print 'Reading input BUFR file: ', input_filename
    bufr = codes_bufr_new_from_file(fbufrin)

    codes_set(bufr, 'masterTablesVersionNumber', 24)
    codes_set(bufr, 'localTablesVersionNumber', 0)
    codes_set(bufr, 'compressedData', 1)
    codes_set(bufr, 'numberOfSubsets', len(years))

    unexpandedDescriptors = [301051, 4006, 7002, 10004, 12001, 11001, 11002,
                             11031, 11032, 11033, 20041]

    codes_set_array(bufr, 'unexpandedDescriptors', unexpandedDescriptors)

    codes_set_double_array(bufr, 'year', years)
    codes_set_double_array(bufr, 'month', months)
    codes_set_double_array(bufr, 'day', days)
    codes_set_double_array(bufr, 'hour', hours)
    codes_set_double_array(bufr, 'minute', minutes)
    codes_set_double_array(bufr, 'second', seconds)
    codes_set_double_array(bufr, 'latitude', latitudes)
    codes_set_double_array(bufr, 'longitude', longitudes)
    codes_set_double_array(bufr, 'height', altitudes)
    codes_set_double_array(bufr, 'pressure', pressures)
    codes_set_double_array(bufr, 'windSpeed', windSpeeds)
    codes_set_double_array(bufr, 'windDirection', windDirections)
    codes_set_double_array(bufr, 'airTemperature', temperatures)

    codes_set(bufr, 'pack', 1)

    codes_write(bufr, fbufrout)
    print 'Created output BUFR file: ', output_filename


def main():
    if len(sys.argv) < 4:
        print >>sys.stderr, 'Usage: ', sys.argv[0], ' csv bufr_in bufr_out'
        sys.exit(1)

    csv_filename = sys.argv[1]
    input_filename = sys.argv[2]
    output_filename = sys.argv[3]

    try:
        example(csv_filename, input_filename, output_filename)
    except CodesInternalError, err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr, err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())
