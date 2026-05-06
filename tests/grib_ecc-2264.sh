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

# here we expect step=0-24 and endStep 24
# 50r1
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=36;
set class='od';
set type='gwt';
set generatingProcessIdentifier=161;
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=2;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0-24"

# here we also expect step=0-24 and endStep 24
# should be fixed for ecPoint types only

# ERA5 gwt
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=35;
set class='ea';
set type='gwt';
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0-24"

# ERA5 gbf
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=35;
set class='ea';
set type='gbf';
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0-24"

# ERA5 pfc
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=35;
set class='ea';
set type='pfc';
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0-24"

# ERA5 ppm
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=35;
set class='ea';
set type='ppm';
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0-24"

# ERA5 fc
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=35;
set class='ea';
set type='fc';
set productDefinitionTemplateNumber=8;
set typeOfStatisticalProcessing=1;
set lengthOfTimeRange=24;
set typeOfTimeIncrement=1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib step "0"

# Clean up
rm -f $tempGrib $tempFilt
