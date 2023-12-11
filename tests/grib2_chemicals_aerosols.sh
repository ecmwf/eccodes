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

label="grib2_chemicals_aerosols_test"
temp=temp.$label
temp1=temp.$label.1
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`

# ECC-1417: Replace key aerosolType with constituentType
# ------------------------------------------------------
${tools_dir}/grib_set -s tablesVersion=$latest,paramId=210072 $sample2 $temp
grib_check_key_equals $temp aerosolType         62026
grib_check_key_equals $temp aerosolTypeName     'Particulate matter'
grib_check_key_equals $temp constituentType     62026
grib_check_key_equals $temp constituentTypeName 'Particulate matter'


# Check latest chemical/aerosol code tables
# -----------------------------------------
${tools_dir}/grib_set -s tablesVersion=$latest,paramId=217224  $sample2 $temp
grib_check_key_equals $temp constituentTypeName "Aceto nitrile CH3CN"

${tools_dir}/grib_set -s tablesVersion=$latest,paramId=210249  $sample2 $temp
grib_check_key_equals $temp aerosolTypeName "Ammonium dry"


# =============================
# Deterministic instantaneous
# =============================
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

# Plain aerosols
${tools_dir}/grib_set -s tablesVersion=$latest,is_aerosol=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '48'
grib_check_key_equals $temp aerosolType,typeOfSizeInterval,typeOfWavelengthInterval '0 0 0'

# Aerosol optical
${tools_dir}/grib_set -s tablesVersion=$latest,is_aerosol_optical=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '48'
#${tools_dir}/grib_dump -O $temp


# =============================
# Deterministic interval-based
# =============================
# Plain chemicals
${tools_dir}/grib_set -s tablesVersion=$latest,stepType=accum,is_chemical=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '42'
grib_check_key_equals $temp constituentType '0'

# Chemicals with source and sink
${tools_dir}/grib_set -s tablesVersion=$latest,stepType=accum,is_chemical_srcsink=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '78'
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess '0 255'

# Chemicals with distribution function
${tools_dir}/grib_set -s tablesVersion=$latest,stepType=accum,is_chemical_distfn=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '67'
grib_check_key_equals $temp constituentType,numberOfModeOfDistribution,modeNumber '0 0 0'

# Plain aerosols
${tools_dir}/grib_set -s tablesVersion=$latest,stepType=accum,is_aerosol=1 $sample2 $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '46'
grib_check_key_equals $temp aerosolType,typeOfSizeInterval '0 0'


# =============================
# Ensemble instantaneous
# =============================
# Plain chemicals
tempSample=temp.sample.$label
${tools_dir}/grib_set -s tablesVersion=29,productDefinitionTemplateNumber=1 $sample2 $tempSample
grib_check_key_equals $tempSample perturbationNumber '0'

${tools_dir}/grib_set -s is_chemical=1 $tempSample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '41'
grib_check_key_equals $temp constituentType,perturbationNumber '0 0'

# Chemicals with source and sink
${tools_dir}/grib_set -s is_chemical_srcsink=1 $tempSample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '77'
grib_check_key_equals $temp constituentType,sourceSinkChemicalPhysicalProcess '0 255'

# Chemicals with distribution function
${tools_dir}/grib_set -s is_chemical_distfn=1 $tempSample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '58'
grib_check_key_equals $temp constituentType,numberOfModeOfDistribution,modeNumber '0 0 0'

# Plain aerosols
${tools_dir}/grib_set -s is_aerosol=1 $tempSample $temp
grib_check_key_equals $temp productDefinitionTemplateNumber '45'
grib_check_key_equals $temp aerosolType,typeOfSizeInterval '0 0'

# Keys firstSize and secondSize
${tools_dir}/grib_set -s paramId=210072 $tempSample $temp
${tools_dir}/grib_ls -p firstSize,secondSize $temp

# ECC-1303: Setting localDefinitionNumber=1 on chemical source/sink
# ------------------------------------------------------------------
${tools_dir}/grib_set -s paramId=228104,setLocalDefinition=1,localDefinitionNumber=1 $sample2 $temp
grib_check_key_equals $temp paramId,productDefinitionTemplateNumber,is_chemical_srcsink,localUsePresent '228104 76 1 1'

${tools_dir}/grib_set -s paramId=228104 $sample2 $temp
grib_check_key_equals $temp shortName,productDefinitionTemplateNumber,is_chemical_srcsink 'e_WLCH4 76 1'
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=1 $temp $temp1
${tools_dir}/grib_compare -b totalLength,numberOfSection $temp $temp1
grib_check_key_equals $temp1 localUsePresent 1

# Check local ECMWF chemicals/aerosols
# ------------------------------------
${tools_dir}/grib_set -s paramId=217232 $sample2 $temp
${tools_dir}/grib_dump -O -p constituentType $temp > $temp1
grep -q "Biomass burning volatile organic compounds .*grib2/tables/local/ecmf/1/4.230.table" $temp1

${tools_dir}/grib_set -s paramId=219231 $sample2 $temp
${tools_dir}/grib_dump -O -p constituentType $temp > $temp1
grep -q "Anthropogenic volatile organic compounds .*grib2/tables/local/ecmf/1/4.230.table" $temp1

${tools_dir}/grib_set -s paramId=211248 $sample2 $temp
${tools_dir}/grib_dump -O -p aerosolType $temp > $temp1
grep -q "Nitrate Coarse Mode .*grib2/tables/local/ecmf/1/4.233.table" $temp1

${tools_dir}/grib_set -s paramId=215189 $sample2 $temp
${tools_dir}/grib_dump -O -p aerosolType $temp > $temp1
grep -q "Nitrate Fine Mode .*grib2/tables/local/ecmf/1/4.233.table" $temp1
grib_check_key_equals $temp aerosolTypeName "Nitrate Fine Mode"

# Clean up
rm -f $tempSample $temp $temp1
