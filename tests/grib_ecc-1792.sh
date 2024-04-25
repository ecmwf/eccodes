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

REDIRECT=/dev/null

label="grib_ecc-1792_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a message with its Data Section out-of-sync with its Grid
${tools_dir}/grib_set -s numberOfValues=2 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib numberOfDataPoints 496

# grib_get_data should fail
set +e
${tools_dir}/grib_get_data $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep "Grid description is wrong" $tempLog

# Just getting lats and lons; no need for Data Section to be in-sync
cat >$tempFilt<<EOF
    print "latitudes  = [latitudes]";
    print "longitudes = [longitudes]";
    print "distinctLatitudes  = [distinctLatitudes]";
    print "distinctLongitudes  = [distinctLongitudes]";
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempGrib

# latLonValues does require Data Section to be in-sync
cat >$tempFilt<<EOF
    print "[latLonValues]";
EOF
set +e
${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempGrib > $tempLog 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep "Grid description is wrong" $tempLog


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempRef
