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
# This is the test for JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempErr=temp.${label}.err

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# configuration in class we - 56 for gfas
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set marsClass = 56;
set marsType = 89;
set marsStream = 1252;
set backgroundProcess = 144;
set generatingProcessIdentifier = 1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib configuration "v1.2"

# no configuration for gfas in class mc - 19
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set marsClass = 19;
set marsType = 89;
set marsStream = 1252;
set backgroundProcess = 144;
set generatingProcessIdentifier = 1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
set +e
${tools_dir}/grib_get -p configuration $tempGrib configuration 2>$tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q "configuration (Key/value not found)" $tempErr

# Clean up
rm -f $tempGrib $tempFilt $tempErr
