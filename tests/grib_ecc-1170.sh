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
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-1170
# grib_filter: Fail if key of type double (e.g. values) is assigned to a string
# ---------------------------------------------------------
label="grib_ecc-1170-test"

tempErr=temp.${label}.err
tempFilt=temp.${label}.filt
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempFilt <<EOF
  set values = "illegal";
EOF
set +e
${tools_dir}/grib_filter $tempFilt $sample2 2>$tempErr
status=$?
set -e
[ $status -ne 0 ]

#cat $tempErr
grep -q "unable to set values as double" $tempErr


# Clean up
rm -f $tempFilt $tempErr
