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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-922
# grib_get_data should fail on incorrectly coded grid
# ---------------------------------------------------------

label="grib_ecc-922_test"
temp=temp.$label
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create an inconsistent grid: j scans negatively but lat1 < lat2
${tools_dir}/grib_set -s latitudeOfFirstGridPoint=0,latitudeOfLastGridPoint=60000000 $sample_grib2 $temp
grib_check_key_equals $temp jScansPositively 0
# grib_get_data should fail
set +e
${tools_dir}/grib_get_data $temp
status=$?
set -e
[ $status -ne 0 ]


rm -f $temp
