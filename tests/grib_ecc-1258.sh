#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# ---------------------------------------------------------------------
# This is the test for JIRA issue ECC-1258
# find_nearest returns wrong point if searched location is at a grid point
# ---------------------------------------------------------------------

label="grib_ecc-1258_test"
tempFilt=temp.$label.filt
tempGrib=temp.$label.grib
tempOut=temp.$label.txt
sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

cat > $tempFilt <<EOF
 set Ni = 16;
 set Nj = 21;
 set latitudeOfFirstGridPoint= -29600;
 set longitudeOfFirstGridPoint = 134000;
 set latitudeOfLastGridPoint= -37600;
 set longitudeOfLastGridPoint= 140000;
 set iDirectionIncrement = 400;
 set jDirectionIncrement= 400;
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib1
${tools_dir}/grib_ls  -l -32.4,137.6  $tempGrib > $tempOut

if [ "${HAVE_ECKIT_GEO:-0}" -ne 1 ] ||
   [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ]; then
    # Use ecCodes native (incorrect)
    # Low precision lat/lon values are used in ecCodes native, so the point is found at the first attempt (index=120)
    grep -q "Grid Point chosen #3 index=121" "$tempOut"
else
    # Use eckit-geo (correct)
    # High precision lat/lon values are used in eckit-geo, so the point is found at the second attempt (index=121)
    grep -q "Grid Point chosen #2 index=121" "$tempOut"
fi

grep -q "index=121 .*distance=0.0" "$tempOut"

rm -f $tempFilt $tempGrib $tempOut
