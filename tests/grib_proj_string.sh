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
  regular_latlon_surface.grib1
  mercator.grib2
  satellite.grib
"

for f in `echo $files`; do
    file=${data_dir}/$f
    ps=`${tools_dir}/grib_get -wcount=1 -p projString $file`
    # Check length of result is nonzero
    [ -n "$ps" ]
done

# Reminder
keysfile=${proj_dir}/tests/keys
grep -q projString $keysfile
