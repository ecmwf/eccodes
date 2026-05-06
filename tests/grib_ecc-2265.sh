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
set productDefinitionTemplateNumber = 93;
set inputOriginatingCentre = 98 ;
set typeOfGeneratingProcess = 2 ;
set setLocalDefinition=1;
set grib2LocalSectionNumber = 1 ;
set marsClass = 39 ;
set marsType = 9 ;
set marsStream = 1025 ;
set experimentVersionNumber = 1;
set typeOfPostProcessing = 11 ;
set tablesVersion=35;
set inputProcessIdentifier=65533;
set backgroundProcess = 146;
set generatingProcessIdentifier = 1;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib forcing "ecmf-era5"

# Clean up
rm -f $tempGrib $tempFilt
