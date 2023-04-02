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

label="grib_ecc-1560_test"
temp_PDTN08_Grib=temp.$label.PDTN08.grib
temp_PDTN11_Grib=temp.$label.PDTN11.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# We set PDTN=8 and level to 1.5 metres above ground
${tools_dir}/grib_set -s \
  productDefinitionTemplateNumber=8,typeOfFirstFixedSurface=103,scaledValueOfFirstFixedSurface=15,scaleFactorOfFirstFixedSurface=1 \
  $sample_grib2 $temp_PDTN08_Grib
grib_check_key_equals $temp_PDTN08_Grib level:d 1.5

# Now we change to PDTN 11
${tools_dir}/grib_set -s productDefinitionTemplateNumber=11 $temp_PDTN08_Grib $temp_PDTN11_Grib
grib_check_key_equals $temp_PDTN11_Grib level:d 1.5

${tools_dir}/grib_compare $temp_PDTN08_Grib $temp_PDTN11_Grib
# Clean up
rm -f $temp_PDTN08_Grib $temp_PDTN11_Grib
