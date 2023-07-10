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

label="grib_ecc-1560_test"
temp_PDTN08_Grib=temp.$label.PDTN08.grib
temp_PDTN11_Grib=temp.$label.PDTN11.grib
tempGrib=temp.$label.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# We set PDTN=8 and level to 1.5 metres above ground
${tools_dir}/grib_set -s \
  productDefinitionTemplateNumber=8,typeOfFirstFixedSurface=103,scaledValueOfFirstFixedSurface=15,scaleFactorOfFirstFixedSurface=1 \
  $sample_grib2 $temp_PDTN08_Grib
grib_check_key_equals $temp_PDTN08_Grib level:d 1.5

# Now we change to PDTN 11
${tools_dir}/grib_set -s productDefinitionTemplateNumber=11 $temp_PDTN08_Grib $temp_PDTN11_Grib
grib_check_key_equals $temp_PDTN11_Grib level:d 1.5
grib_check_key_equals $temp_PDTN11_Grib scaledValueOfFirstFixedSurface 15
grib_check_key_equals $temp_PDTN11_Grib scaleFactorOfFirstFixedSurface 1


# Set whole value using the int/double encoding
# -----------------------------------------------
${tools_dir}/grib_set -s typeOfFirstFixedSurface=105,level:i=34 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface 0
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface 34

${tools_dir}/grib_set -s typeOfFirstFixedSurface=105,level:d=34 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface 0
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface 34

${tools_dir}/grib_set -s typeOfFirstFixedSurface=105,level:d=3.456 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface 3
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface 3456


# Clean up
rm -f $temp_PDTN08_Grib $temp_PDTN11_Grib $tempGrib
