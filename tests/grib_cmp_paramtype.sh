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
# This is the test for the JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_cmp_paramtype_test"  # Change prod to bufr or grib etc
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# wave_spectra
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=99;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "wave_spectra"

# wave
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set discipline=10;
set parameterCategory=0;
set parameterNumber=3;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "wave"

# chem in ERA6 as paramtype base
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=50;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "base"

# chem in CAMS as paramtype chemical
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='mc';
set tablesVersion=34;
set productDefinitionTemplateNumber=50;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "chemical"

# chem/optical in ERA6 as paramtype optical
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=48;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "optical"

# chem/optical in CAMS as paramtype chemical_optical
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='mc';
set tablesVersion=34;
set productDefinitionTemplateNumber=48;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "chemical_optical"


# base
cat >$tempFilt<<EOF
set setLocalDefinition=1;
set class='e6';
set tablesVersion=34;
set productDefinitionTemplateNumber=0;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramtype "base"

# Clean up
rm -f $tempGrib $tempFilt 
