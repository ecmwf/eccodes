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

# ECC-1766: EERIE project metadata support

label="grib_ecc-1766_test"
temp_grib=temp.$label.grib
eerie_sample=temp.$label.eerie.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

tablesVersionLatest=$( ${tools_dir}/grib_get -p tablesVersionLatest $sample_grib2 )

# Setup GRIB message with MARS keys
# Set latest tables version and add local section with MARS labeling
${tools_dir}/grib_set -s tablesVersion=$tablesVersionLatest,setLocalDefinition=1 $sample_grib2 $temp_grib

# Then change to EERIE project GRIB message --> grib2LocalSectionNumber=43
${tools_dir}/grib_set -s grib2LocalSectionNumber=43,class=ed $temp_grib $eerie_sample

# Check EERIE related keys are present and correct
grib_check_key_exists $eerie_sample activity,experiment,realization
grib_check_key_equals $eerie_sample "activity,experiment,realization" "0 0 255"

# Check domain key is unaliased
[ $( ${tools_dir}/grib_get -f -p mars.domain $eerie_sample ) = "not_found" ]

# Check an example where a few additional things are set
${tools_dir}/grib_set -s activity=1,experiment=1,realization=1 $eerie_sample $temp_grib

grib_check_key_equals $temp_grib "activity,experiment,realization" "1 1 1"
grib_check_key_equals $temp_grib "activity:s,experiment:s" "CMIP6 hist"

# Clean up
rm -f $temp_grib $eerie_sample
