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
# This is the test for JIRA issue ECC-2230
# GRIB2: Potential rounding bug in the 'wavelength' key
# ---------------------------------------------------------

label=`basename $0 | sed -e 's/\.sh/_test/'`

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
cat >$tempFilt<<EOF
  set class = 'mc';
  set tablesVersion = 35;
  set productDefinitionTemplateNumber = 48;
  set paramId = 457000;
  set typeOfSizeInterval = 3;
  set scaleFactorOfFirstSize = 0;
  set scaledValueOfFirstSize = 0;
  set typeOfWavelengthInterval = 11;
  set scaleFactorOfFirstWavelength = 9;
  set scaledValueOfFirstWavelength = 469;
  set enableChemSplit = 1;
  set chemId = 922;
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
result=$( ${tools_dir}/grib_get -p wavelength:d -F%.15f $tempGrib )
[ "$result" = "469.000000000000000" ] || exit 1

grib_check_key_equals $tempGrib "wavelength" "469"

# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef
