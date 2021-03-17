#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u

label="grib_ecc-1224-test"
temp=temp.$label
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`

# CHEMICALS
# -----------
# Plain chemicals
${tools_dir}/grib_set -s tablesVersion=$latest,is_chemical=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '40'
grib_check_key_equals $temp constituentType '0'

# Chemicals with source and sink
${tools_dir}/grib_set -s tablesVersion=$latest,is_chemical_srcsink=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '76'
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess '0 255'

# Chemicals with distribution function
${tools_dir}/grib_set -s tablesVersion=$latest,is_chemical_distfn=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '57'
grib_check_key_equals $temp constituentType,numberOfModeOfDistribution,modeNumber '0 0 0'

# AEROSOLS
# ----------
# Plain aerosols
${tools_dir}/grib_set -s tablesVersion=$latest,is_aerosol=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '48'
grib_check_key_equals $temp aerosolType,typeOfSizeInterval,typeOfWavelengthInterval '0 0 0'

# Aerosol optical
${tools_dir}/grib_set -s tablesVersion=$latest,is_aerosol_optical=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '48'
#${tools_dir}/grib_dump -O $temp


rm -f $temp
