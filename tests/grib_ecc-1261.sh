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

label="grib_ecc-1261_test"

tempFilt="temp.${label}.filt"
tempGrib="temp.${label}.grib"

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

test_ld12()
{
    samplef=$1
    itime=$2 # input indexingTime
    otime=$3 # output (expected)

    cat > $tempFilt << EOF
      set setLocalDefinition=1;
      set localDefinitionNumber=12;
      set indexingTime = $itime;
      write;
EOF
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $samplef
    grib_check_key_equals $tempGrib "mars.time" "$otime"
    rm -f $tempFilt $tempGrib
}

test_ld12 $sample_grib1 "0"    "0000"
test_ld12 $sample_grib1 "12"   "0012"
test_ld12 $sample_grib1 "2400" "2400"
test_ld12 $sample_grib1 "1423" "1423"

test_ld12 $sample_grib2 "0"    "0000"
test_ld12 $sample_grib2 "12"   "0012"
test_ld12 $sample_grib2 "2400" "2400"
test_ld12 $sample_grib2 "1423" "1423"

