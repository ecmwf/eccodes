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
# This is the test for JIRA issue ECC-1734
# GRIB2: typeOfTimeIncrement=1 removes endStep and stepRange
# ---------------------------------------------------------

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

tablesVersionLatest=$( ${tools_dir}/grib_get -p tablesVersionLatest $sample_grib2 )

# Set endStep and typeOfTimeIncrement=1. This should result in endStep and stepRange being set to 0 and 0-6 respectively
cat >$tempFilt<<EOF
    set tablesVersion=$tablesVersionLatest;
    set productDefinitionTemplateNumber=8;
    set startStep=0;
    set endStep=6;
    set typeOfTimeIncrement=1;
    write;
EOF

# Check that the values for these keys are computed correctly
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib endStep,stepRange "6 0-6"

# Clean up
rm -f $tempGrib $tempFilt
