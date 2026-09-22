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
# This is the test for JIRA issue ECC-2268
# grib_compare -r does not reorder data correctly
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib_34=temp.$label.34.grib
tempGrib_134=temp.$label.134.grib
tempGrib_34_134=temp.$label.34_134.grib
tempGrib_134_34=temp.$label.134_34.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s paramId=34  $sample_grib2 $tempGrib_34
${tools_dir}/grib_set -s paramId=134 $sample_grib2 $tempGrib_134
cat $tempGrib_34  $tempGrib_134 > $tempGrib_34_134
cat $tempGrib_134 $tempGrib_34  > $tempGrib_134_34

${tools_dir}/grib_compare -r $tempGrib_34_134 $tempGrib_134_34

md5_34=$(  ${tools_dir}/grib_get -p md5Headers $tempGrib_34  )
md5_134=$( ${tools_dir}/grib_get -p md5Headers $tempGrib_134 )
if [ "$md5_34" == "$md5_134" ]; then
    echo "Error: md5Headers should be different for paramId=34 and paramId=134"
    echo "md5Headers for paramId=34:  $md5_34"
    echo "md5Headers for paramId=134: $md5_134"
    exit 1
fi

# Clean up
rm -f $tempGrib_34 $tempGrib_134 $tempGrib_34_134 $tempGrib_134_34

