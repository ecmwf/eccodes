#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

# Define a common label for all the tmp files
label="grib_mercator_test"
tempOut="temp.${label}.out"
tempFilt="temp.${label}.filt"

input=${data_dir}/mercator.grib2
grib_check_key_equals $input "gridType" "mercator"
${tools_dir}/grib_get_data $input > $tempOut
numlines=`wc -l $tempOut | awk '{print $1}'`
[ "$numlines" = "2465" ]     # 1 + numberOfDataPoints


# Check other iterator-related keys
cat > $tempFilt <<EOF
 print "[latLonValues]";
 print "[distinctLatitudes]";
 print "[distinctLongitudes]";
EOF

${tools_dir}/grib_filter $tempFilt $input > $tempOut

# Nearest function
${tools_dir}/grib_ls -l 19,-97,1 $input > $tempOut
grep -q "Point chosen #1 index=618" $tempOut

# Error conditions
# ----------------
input=${data_dir}/mercator.grib2
set +e
${tools_dir}/grib_get_data -s latitudeOfFirstGridPointInDegrees=90 $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Transformation cannot be computed at the poles" $tempOut

input=${data_dir}/mercator.grib2
set +e
${tools_dir}/grib_get_data -s Ni=1 $input > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong number of points" $tempOut


# Clean up
rm -f $tempFilt $tempOut
