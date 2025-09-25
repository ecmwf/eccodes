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
# This is the test for JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_ecc-2140_test"  # Change prod to bufr or grib etc
tempGrib=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# pre-mtg2
${tools_dir}/grib_set -s tablesVersion=32,setLocalDefinition=1,class=cr,generatingProcessIdentifier=158,productDefinitionTemplateNumber=48,typeOfWavelengthInterval=11,scaledValueOfFirstWavelength=500,scaleFactorOfFirstWavelength=9 $sample_grib2 $tempGrib
[ $( ${tools_dir}/grib_get -f -p mars.wavelength $tempGrib ) = "not_found" ]

# 50r1
${tools_dir}/grib_set -s tablesVersion=35,setLocalDefinition=1,class=cr,generatingProcessIdentifier=161,productDefinitionTemplateNumber=48,typeOfWavelengthInterval=11,scaledValueOfFirstWavelength=500,scaleFactorOfFirstWavelength=9 $sample_grib2 $tempGrib
[ $( ${tools_dir}/grib_get -f -p mars.wavelength $tempGrib ) = "not_found" ]

# 50r2
${tools_dir}/grib_set -s tablesVersion=35,setLocalDefinition=1,class=cr,generatingProcessIdentifier=162,productDefinitionTemplateNumber=48,typeOfWavelengthInterval=11,scaledValueOfFirstWavelength=500,scaleFactorOfFirstWavelength=9 $sample_grib2 $tempGrib
[ $( ${tools_dir}/grib_get -f -p mars.wavelength $tempGrib ) = 500 ]

# Clean up
rm -f $tempGrib
