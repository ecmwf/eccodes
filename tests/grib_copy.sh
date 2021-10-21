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

# Enter data dir
cd ${data_dir}

# Define a common label for all the tmp files
label="grib_copy_test"
temp=${label}".grib.tmp"

echo "Test: The -g option..."
# -----------------------------------
input=gts.grib
${tools_dir}/grib_copy -w count=1 -g $input $temp
val1=`${tools_dir}/gts_get -wcount=1 -p AA $input`
val2=`${tools_dir}/gts_get -p AA $temp`
[ "$val1" = "$val2" ]
[ "$val1" = "XK" ]

${tools_dir}/grib_copy -w count=1 $input $temp
set +e
${tools_dir}/gts_get -p AA $temp
status=$?
set -e
[ $status -ne 0 ]


echo "Test: The -X option..."
# -------------------------------
echo "Test: use of -X option"
input=tigge_pf_ecmwf.grib2
${tools_dir}/grib_copy -w count=1 -X 0 $input $temp #First msg
r1=`${tools_dir}/grib_get -w count=1 -n ls $input`
r2=`${tools_dir}/grib_get -n ls $temp`
[ "$r1" = "$r2" ]


echo "Test: ECC-1086 invalid message ..."
# -------------------------
# This file is 179 bytes long. We chop the last byte to create
# an invalid GRIB message (Final 7777 is 777)
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
badGrib=${label}".bad.grib"
head -c 178 $input > $badGrib
set +e
${tools_dir}/grib_copy $badGrib /dev/null  # Only the bad GRIB
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_copy $input $badGrib /dev/null # Good followed by bad
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_copy $badGrib $input /dev/null # Bad followed by good
status=$?
set -e
[ $status -ne 0 ]

# If there is a bad message, make sure the rest (good ones) get copied
combinedGrib=${label}".combined.grib"
cat $badGrib tigge_cf_ecmwf.grib2 > $combinedGrib
set +e
${tools_dir}/grib_copy $combinedGrib $temp
status=$?
set -e
[ $status -ne 0 ]
count=`${tools_dir}/grib_count $temp`
[ $count -eq 43 ]


echo "Test: ECC-539 input=output ..."
# -----------------------
set +e
${tools_dir}/grib_copy  $temp $temp
status=$?
set -e
[ $status -ne 0 ]


#${tools_dir}/grib_copy -w count=1 -X 57143 $input $temp #Last msg
#r1=`${tools_dir}/grib_get -w count=37 -n ls $input`
#r2=`${tools_dir}/grib_get -n ls $temp`
#[ "$r1" = "$r2" ]

# Get two messages
#${tools_dir}/grib_copy -w count=1/2 -X 25071 $input $temp
#count=`${tools_dir}/grib_count $temp`
#[ $count -eq 2 ]

#r=`${tools_dir}/grib_get -w count=1 -p typeOfLevel,level,shortName $temp`
#[ "$r" = "heightAboveGround 2 mx2t6" ]
#r=`${tools_dir}/grib_get -w count=2 -p typeOfLevel,level,shortName $temp`
#[ "$r" = "heightAboveGround 2 mn2t6" ]

# Clean up
#-----------
rm -f $temp $badGrib $combinedGrib
