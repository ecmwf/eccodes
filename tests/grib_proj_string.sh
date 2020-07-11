#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

files="
  mercator.grib2
  satellite.grib
"

# Decide if we have the proj commandline tool
PROJ_NAME="proj"
PROJ_TOOL=""
if command -v $PROJ_NAME >/dev/null 2>&1; then
    PROJ_TOOL=$PROJ_NAME
fi

for f in `echo $files`; do
    file=${data_dir}/$f
    ps=`${tools_dir}/grib_get -wcount=1 -p projSourceString $file`
    [ "$ps" = "EPSG:4326" ]
    ps=`${tools_dir}/grib_get -wcount=1 -p projTargetString $file`
    # Check length of result is nonzero
    [ -n "$ps" ]
    # Check contents (basic)
    case $ps in
      *+proj=*) echo OK;;
      *)        echo "File: $file. Invalid proj string: |$ps|"; exit 1;;
    esac
    if test "x$PROJ_TOOL" != "x"; then
        ${tools_dir}/grib_get -p longitudeOfFirstGridPointInDegrees,latitudeOfFirstGridPointInDegrees $file |\
            $PROJ_TOOL $ps
    fi
done

# Reminder
keysfile=${proj_dir}/tests/keys
grep -q projTargetString $keysfile
