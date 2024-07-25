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

label="grib2_paramtype_test"
tempGrib=temp.$label.grib

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

check_param_type_is_scalar()
{
    # Check we do not end up with an array for paramtype
    infile=$1
    ptype=$2
    result=$(echo 'print "|[paramtype]|";' | ${tools_dir}/grib_filter - $infile)
    [ "$result" = "|$ptype|" ]
}

# Base
${tools_dir}/grib_set -s shortName=t $sample $tempGrib
grib_check_key_equals $tempGrib mars.paramtype 'base'
check_param_type_is_scalar $sample 'base'

# Aerosols
${tools_dir}/grib_set -s productDefinitionTemplateNumber=45,discipline=0,parameterCategory=20,parameterNumber=0 \
 $sample $tempGrib
grib_check_key_equals $tempGrib mars.paramtype 'base'
check_param_type_is_scalar $tempGrib 'base'

# Atmospheric chemical constituents
${tools_dir}/grib_set -s productDefinitionTemplateNumber=40,discipline=0,parameterCategory=20,parameterNumber=2 \
 $sample $tempGrib
grib_check_key_equals $tempGrib mars.paramtype 'base'
check_param_type_is_scalar $tempGrib 'base'


# Optical properties of aerosol
${tools_dir}/grib_set -s productDefinitionTemplateNumber=48,discipline=0,parameterCategory=20,parameterNumber=102 \
 $sample $tempGrib
${tools_dir}/grib_dump -O -p section_4 $tempGrib > /dev/null
grib_check_key_equals $tempGrib mars.paramtype 'optical'
check_param_type_is_scalar $tempGrib 'optical'

# Generalised tiles
${tools_dir}/grib_set -s productDefinitionTemplateNumber=113 $sample $tempGrib
grib_check_key_equals $tempGrib mars.paramtype 'tile'
check_param_type_is_scalar $tempGrib 'tile'

# Wave spectra
for pdtn in 99 100 101 102; do
    ${tools_dir}/grib_set -s productDefinitionTemplateNumber=$pdtn $sample $tempGrib
    grib_check_key_equals $tempGrib mars.paramtype 'wave_spectra'
    check_param_type_is_scalar $tempGrib 'wave_spectra'
done

# Wave
for pid in 140200 140250; do
    ${tools_dir}/grib_set -s paramId=$pid $sample $tempGrib
    grib_check_key_equals $tempGrib mars.paramtype 'wave'
    check_param_type_is_scalar $tempGrib 'wave'
done

# Clean up
rm -f $tempGrib
