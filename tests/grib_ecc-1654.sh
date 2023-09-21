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

# ECC-1654: Error setting key of type 'codetable' as a string

label="grib_ecc-1654_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s typeOfFirstFixedSurface:s=15 $sample_grib2 $tempGribA
grib_check_key_equals $tempGribA 'typeOfFirstFixedSurface:i' '15'
${tools_dir}/grib_dump -O -p section_4 $tempGribA > $tempLog
grep -q 'typeOfFirstFixedSurface = 15 .*Unknown code table entry' $tempLog

${tools_dir}/grib_set -s productDefinitionTemplateNumber=0,typeOfFirstFixedSurface=15 $sample_grib2 $tempGribA
${tools_dir}/grib_set -s productDefinitionTemplateNumber=1 $tempGribA $tempGribB
${tools_dir}/grib_compare -b totalLength,section4Length,productDefinitionTemplateNumber $tempGribA $tempGribB

# Clean up
rm -f $tempGribA $tempGribB $tempFilt $tempLog $tempRef
