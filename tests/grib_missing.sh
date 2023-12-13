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

REDIRECT=/dev/null

label="grib_missing_test"

infile="${data_dir}/missing.grib2"
outfile="${data_dir}/temp.$label.grib2"
tempText=temp.$label.txt
tempGrib=temp.$label.grib

scaleFactorOfSecondFixedSurface=`${tools_dir}/grib_get -w count=1 -p scaleFactorOfSecondFixedSurface $infile`
[ "$scaleFactorOfSecondFixedSurface" = "0" ]

scaledValueOfSecondFixedSurface=`${tools_dir}/grib_get -w count=1 -p scaledValueOfSecondFixedSurface $infile`
[ "$scaledValueOfSecondFixedSurface" = "0" ]

${tools_dir}/grib_set -s scaleFactorOfSecondFixedSurface=missing,scaledValueOfSecondFixedSurface=missing $infile $outfile

scaleFactorOfSecondFixedSurface=`${tools_dir}/grib_get -w count=1 -p scaleFactorOfSecondFixedSurface $outfile`
[ "$scaleFactorOfSecondFixedSurface" = "MISSING" ]

scaledValueOfSecondFixedSurface=`${tools_dir}/grib_get -w count=1 -p scaledValueOfSecondFixedSurface $outfile`
[ "$scaledValueOfSecondFixedSurface" = "MISSING" ]

# Codetable keys being set to 'missing'
# -----------------------------------------
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Make sure it works with the default sample
${tools_dir}/grib_set -s typeOfFirstFixedSurface=missing $sample2 $outfile
grib_check_key_equals $outfile 'typeOfFirstFixedSurface:i' '255'

# Make sure it works with the latest GRIB2 version code table 4.5
latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`
${tools_dir}/grib_set -s tablesVersion=$latest $sample2 $tempGrib
${tools_dir}/grib_set -s typeOfFirstFixedSurface=missing $tempGrib $outfile
grib_check_key_equals $outfile 'typeOfFirstFixedSurface:i' '255'
rm -f $tempGrib

${tools_dir}/grib_set -s centre=missing $sample1 $outfile
grib_check_key_equals $outfile 'centre' 'consensus'

# Some code tables do not have a missing entry
set +e
${tools_dir}/grib_set -s timeRangeIndicator=missing $sample1 $outfile 2>$tempText
status=$?
set -e
[ $status -ne 0 ]
grep -q "There is no 'missing' entry in Code Table 5.table" $tempText


# Clean up
rm -f $outfile $tempText $tempGrib
