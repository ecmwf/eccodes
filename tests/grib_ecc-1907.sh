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

label="grib_ecc-1907_test"
tempGrib1=temp.$label.grib1
tempGrib2=temp.$label.grib2
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

cat >$tempFilt<<EOF
    set localDefinitionNumber = 13;
    set marsStream = 'ewla';
    set directionNumber = 1;
    set frequencyNumber = 1;
    set numberOfDirections = 36;
    set numberOfFrequencies = 36;
    set perturbationNumber = 44;

    set scaledDirections={5000,15000,25000,35000,45000,55000,65000,75000,
        85000,95000,105000,115000,125000,135000,145000,155000,
        165000,175000,185000,195000,205000,215000,225000,235000,
        245000,255000,265000,275000,285000,295000,305000,315000,
        325000,335000,345000,355000};

    set scaledFrequencies={34523,37975,41772,45950,50545,55599,61159,67275,
        74002,81403,89543,98497,108347,119182,131100,144210,
        158631,174494,191943,211138,232252,255477,281024,309127,
        340039,374043,411448,452593,497852,547637,602401,662641,
        728905,801795,881975,970172};

    write;
EOF

${tools_dir}/grib_filter -o $tempGrib1 $tempFilt $sample_grib1
${tools_dir}/grib_set -s edition=2 $tempGrib1 $tempGrib2

echo 'print "[scaledDirections=]"; print; print "[scaledFrequencies=]";' | ${tools_dir}/grib_filter - $tempGrib2 

${tools_dir}/grib_compare -c scaledDirections,scaledFrequencies $tempGrib1 $tempGrib2
${tools_dir}/grib_dump -O $tempGrib2

grib_check_key_equals $tempGrib2 productDefinitionTemplateNumber 100
grib_check_key_equals $tempGrib2 numberOfWaveDirections,numberOfWaveFrequencies '36 36'
grib_check_key_equals $tempGrib2 waveDirectionNumber,waveFrequencyNumber,perturbationNumber '1 1 44'

# Clean up
rm -f $tempGrib1 $tempGrib2 $tempFilt $tempLog $tempOut $tempRef
