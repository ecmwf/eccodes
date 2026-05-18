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

label="grib_codedValues_as_bytes_test"
tempSample=temp.in.$label.tmpl
tempBin=temp.$label.bin
tempOutGrib=temp.out.$label.grib

if [ $HAVE_AEC -eq 0 ]; then
    exit 0
fi

sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -r -s packingType=grid_ccsds $sample_g2 $tempSample

input=${data_dir}/ccsds.grib2
$EXEC ${test_dir}/grib_codedValues_as_bytes $input $tempSample $tempBin $tempOutGrib

# ${tools_dir}/grib_dump -O $tempOutGrib

${tools_dir}/grib_compare -c data:n  $input $tempOutGrib
valid=$( ${tools_dir}/grib_get -s messageValidityChecks=data -p isMessageValid $tempOutGrib )
[ $valid -eq 1 ]


# Clean up
rm -f $tempSample $tempOutGrib $tempBin
