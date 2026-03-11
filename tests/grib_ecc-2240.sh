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

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
set setLocalDefinition=1;
set grib2LocalSectionNumber=36;
set marsStream="xwda";
#set type="fc";
set hoursAfterDataCutoff=11;
set offsetToEndOf4DvarWindow=9;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib mars.obscutoff,mars.anoffset "200 9"

cat >$tempFilt<<EOF
set setLocalDefinition=1;
set grib2LocalSectionNumber=36;
set marsStream="xwda";
#set type="fc";
set hoursAfterDataCutoff=9;
set offsetToEndOf4DvarWindow=9;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib mars.obscutoff,mars.anoffset "0 9"

cat >$tempFilt<<EOF
set setLocalDefinition=1;
set grib2LocalSectionNumber=36;
set marsStream="xwda";
#set type="fc";
set hoursAfterDataCutoff=8;
set offsetToEndOf4DvarWindow=9;
write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib mars.obscutoff,mars.anoffset "0 9"

# Clean up
rm -f $tempGrib $tempFilt

