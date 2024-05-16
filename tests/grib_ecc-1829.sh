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

# ECC-1829: Add MARS support for stream & type combinations required for AIFS ensemble

label="grib_ecc-1829_test"
temp_grib=temp.$label.grib
mars_sample=temp.$label.mars.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Setup GRIB2 message with MARS keys
${tools_dir}/grib_set -s setLocalDefinition=1,grib2LocalSectionNumber=1 $sample_grib2 $mars_sample

# Now set ensemble related keys and check number key is present
${tools_dir}/grib_set -s stream=oper,type=pf,productDefinitionTemplateNumber=1 $mars_sample $temp_grib
grib_check_key_exists $temp_grib mars.number

# Now set keys and check stepRange is used for mars.step for o2d and o3d fields, and that number is still present
${tools_dir}/grib_set -s stream=oper,type=pf,productDefinitionTemplateNumber=11,param=263101,typeOfLevel=oceanSurface,startStep=0,endStep=6 $mars_sample $temp_grib
grib_check_key_exists $temp_grib mars.number
grib_check_key_equals $temp_grib "step" "0-6"
${tools_dir}/grib_set -s stream=oper,type=pf,productDefinitionTemplateNumber=11,param=263501,typeOfLevel=oceanModel,level=1,startStep=0,endStep=6 $mars_sample $temp_grib
grib_check_key_exists $temp_grib mars.number
grib_check_key_equals $temp_grib "step" "0-6"

# Now set keys and check stepRange is used for type efi data (just need any template with stat proc keys)
${tools_dir}/grib_set -s stream=oper,type=efi,productDefinitionTemplateNumber=12,startStep=0,endStep=6 $mars_sample $temp_grib
grib_check_key_equals $temp_grib "step" "0-6"

# Now set keys and check stepRange is used for type em data
${tools_dir}/grib_set -s stream=oper,type=em,productDefinitionTemplateNumber=12,startStep=0,endStep=6 $mars_sample $temp_grib
grib_check_key_equals $temp_grib "step" "0-6"

# Now set keys and check stepRange is used for type es data
${tools_dir}/grib_set -s stream=oper,type=es,productDefinitionTemplateNumber=12,startStep=0,endStep=6 $mars_sample $temp_grib
grib_check_key_equals $temp_grib "step" "0-6"

# Clean up
rm -f $temp_grib $mars_sample
