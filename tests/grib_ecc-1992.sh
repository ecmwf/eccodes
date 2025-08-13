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
# This is the test for JIRA issue ECC-1992
# Concept array mapping
# ---------------------------------------------------------

label="grib_ecc-1992_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempDir=temp.$label.dir

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Use a temporary directory
rm -rf $tempDir
mkdir $tempDir
cd $tempDir
mkdir -p defs/grib2
cat > defs/grib2/paramId.def << EOF
'666666' = {
   discipline = 0 ;
   parameterCategory = 0 ;
   parameterNumber = 0 ;
   typeOfStatisticalProcessing = [3,1,4] ;
}
EOF

# Create the test GRIB2 file
cat > $tempFilt << EOF
    set tablesVersion = 34;
    set productDefinitionTemplateNumber = 8;
    set discipline = 0;
    set parameterCategory = 0;
    set parameterNumber = 0;
    set numberOfTimeRange = 3;
    set typeOfStatisticalProcessing={3,1,4};
    write;
EOF

export ECCODES_DEFINITION_PATH=$PWD/defs
export ECCODES_DEBUG=1
${tools_dir}/grib_set -s tablesVersion=34,paramId=666666 $sample_grib2 $tempGrib > $tempLog 2>&1
grep  'Concept: Key typeOfStatisticalProcessing not found, setting PDTN' $tempLog
unset ECCODES_DEBUG

grib_check_key_equals $tempGrib paramId,productDefinitionTemplateNumber,numberOfTimeRanges '666666 8 3'


rm -f $tempGrib
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib paramId '666666'


# Clean up
cd $test_dir
rm -rf $tempDir
rm -f $tempGrib $tempFilt $tempLog
