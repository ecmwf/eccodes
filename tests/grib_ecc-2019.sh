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

# -------------------------------------------------------------------
# This is the test for the JIRA issue ECC-2019
# Cannot set levelist after setting typeOfLevel=isobaricInhPa
# -------------------------------------------------------------------

label="grib_ecc-2019_test"
tempGrib=temp.$label.grib
tempGribA=temp.$label.a.grib
tempGribB=temp.$label.b.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Sample is for surface and therefore should not have mars.levelist
res=$( ${tools_dir}/grib_get -fp levelist $sample_grib2 )
[ "$res" = "not_found" ]

# Change the typeOfLevel
# First change on disk
${tools_dir}/grib_set -s typeOfLevel=isobaricInhPa $sample_grib2 $tempGribA
${tools_dir}/grib_set -s levelist=900 $tempGribA $tempGribB
grib_check_key_equals $tempGribB levelist,mars.levelist "900 900"

# Change in memory
cat >$tempFilt<<EOF
    set typeOfLevel="isobaricInhPa";
    set level=42;
    print "[levelist=]";
    assert( levelist == 42 );
    write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2
grib_check_key_equals $tempGrib mars.levelist "42"

${tools_dir}/grib_set -s typeOfLevel=isobaricInhPa,levelist=900 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib mars.levelist "900"

# Check keys iterator too
$EXEC $test_dir/grib_ecc-2019

# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef $tempGribA $tempGribB
