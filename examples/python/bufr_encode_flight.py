# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

# Description: how to encode flight dataset into BUFR

from __future__ import print_function
from datetime import datetime
import traceback
import numpy as np
import sys

from eccodes import *

VERBOSE = 1  # verbose error reporting


def example(csvfile, input_filename, output_filename):
    fbufrin = open(input_filename, 'rb')
    fbufrout = open(output_filename, 'wb')

    print('Using ecCodes version: ', codes_get_api_version())

    # The first line in the CSV has the column names
    print('Reading input CSV file: ', csvfile)
    parse_date = lambda x: datetime.strptime(x, '%Y%m%d')
    parse_time = lambda x: datetime.strptime(x, '%H:%M:%S')
    data = np.genfromtxt(csvfile, delimiter=',', dtype=None, names=True,
                         converters={0: parse_date, 1: parse_time})

    ymd_column = data['ymd']
    years = np.array([x.year for x in ymd_column])
    months = np.array([x.month for x in ymd_column])
    days = np.array([x.day for x in ymd_column])

    time_column = data['time']
    hours = np.array([x.hour for x in time_column])
    minutes = np.array([x.minute for x in time_column])
    seconds = np.array([x.second for x in time_column])

    latitudes = data['latitude']
    longitudes = data['longitude']
    altitudes = data['altitude']
    pressures = data['pressure']
    windSpeeds = data['windSpeed']
    windDirections = data['windDirection']
    temperatures = data['temperature']

    print('Reading input BUFR file: ', input_filename)
    bufr = codes_bufr_new_from_file(fbufrin)

    codes_set(bufr, 'masterTablesVersionNumber', 24)
    codes_set(bufr, 'localTablesVersionNumber', 0)
    codes_set(bufr, 'compressedData', 1)
    codes_set(bufr, 'numberOfSubsets', len(years))

    # unexpandedDescriptors and BufrTemplate can be set alternatively
    # to choose the template for the BUFR message

    # unexpandedDescriptors = [301051,4006,7002,10004,12001,11001,11002,11031,11032,11033,20041]
    # codes_set_array(bufr, 'unexpandedDescriptors', unexpandedDescriptors)

    codes_set(bufr, 'BufrTemplate', 'aircraftReportWithSecondsAndPressure')

    codes_set_array(bufr, 'year', years)
    codes_set_array(bufr, 'month', months)
    codes_set_array(bufr, 'day', days)
    codes_set_array(bufr, 'hour', hours)
    codes_set_array(bufr, 'minute', minutes)
    codes_set_array(bufr, 'second', seconds)
    codes_set_array(bufr, 'latitude', latitudes)
    codes_set_array(bufr, 'longitude', longitudes)
    codes_set_array(bufr, 'height', altitudes)
    codes_set_array(bufr, 'nonCoordinatePressure', pressures)
    codes_set_array(bufr, 'windSpeed', windSpeeds)
    codes_set_array(bufr, 'windDirection', windDirections)
    codes_set_array(bufr, 'airTemperature', temperatures)

    codes_set(bufr, 'pack', 1)

    codes_write(bufr, fbufrout)
    print('Created output BUFR file: ', output_filename)


def main():
    if len(sys.argv) < 4:
        print('Usage: ', sys.argv[0], ' csv bufr_in bufr_out', file=sys.stderr)
        sys.exit(1)

    csv_filename = sys.argv[1]
    input_filename = sys.argv[2]
    output_filename = sys.argv[3]

    try:
        example(csv_filename, input_filename, output_filename)
    except CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')

        return 1


if __name__ == "__main__":
    sys.exit(main())
