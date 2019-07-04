#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-136.
# grib_compare ignores difference in indicatorOfTypeOfLevel
# ---------------------------------------------------------
label="grib_ecc-136-test"
temp1=temp1.${label}
temp2=temp2.${label}
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl


# GRIB1: Both 2 and 3 have string rep of "sfc"
# -----------------------------------------------
${tools_dir}/grib_set -s indicatorOfTypeOfLevel=2 $sample1 $temp1
${tools_dir}/grib_set -s indicatorOfTypeOfLevel=3 $sample1 $temp2

# This should fail
set +e
${tools_dir}/grib_compare $temp1 $temp2
status=$?
set -e
[ $status -eq 1 ]


# GRIB2: Both 1 and 103 have string rep of "sfc"
# -----------------------------------------------
${tools_dir}/grib_set -s typeOfFirstFixedSurface=1   $sample2 $temp1
${tools_dir}/grib_set -s typeOfFirstFixedSurface=103 $sample2 $temp2

# This should fail
set +e
${tools_dir}/grib_compare $temp1 $temp2
status=$?
set -e
[ $status -eq 1 ]


# Clean up
rm -f $temp1 $temp2
