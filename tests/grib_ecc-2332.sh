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
# This is the test for JIRA issue ECC-2332
# Ensure concepts defining probabilityType + typeOfRelationToReferenceDataset
# select PDT 4.131 for instant and PDT 4.112 for interval data.
# ---------------------------------------------------------

label="grib_ecc-2332_test"
temp_instant=temp.${label}.instant.grib
temp_interval=temp.${label}.interval.grib

sample_grib2=${samp_dir}/GRIB2.tmpl

# Example concept entry with both keys exists for tablesVersion=37:
# paramId=133093

# Instantaneous case -> PDT 4.131
${tools_dir}/grib_set -s tablesVersion=37,stepType=instant,paramId=133093 $sample_grib2 $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType,probabilityType,typeOfRelationToReferenceDataset '131 instant 3 1'

# Time-interval case -> PDT 4.112
${tools_dir}/grib_set -s tablesVersion=37,stepType=avg,paramId=133093 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,probabilityType,typeOfRelationToReferenceDataset '112 3 1'

${tools_dir}/grib_set -s tablesVersion=37,stepType=instant,paramId=171256 $sample_grib2 $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType,paramId,typeOfRelationToReferenceDataset '128 instant 171256 0'

${tools_dir}/grib_set -s tablesVersion=37,stepType=avg,paramId=171256 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,stepType,paramId,typeOfRelationToReferenceDataset '105 avg 171256 0'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=1,perturbationNumber=7,stepType=instant,paramId=171256 $sample_grib2 $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType,paramId,typeOfRelationToReferenceDataset,perturbationNumber '129 instant 171256 0 7'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=1,perturbationNumber=7,stepType=avg,paramId=171256 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,stepType,paramId,typeOfRelationToReferenceDataset,perturbationNumber '106 avg 171256 0 7'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=8,paramId=171256 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber '105'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=11,perturbationNumber=7,paramId=171256,startDateOfReferencePeriod=20000101 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,perturbationNumber,is_referenceperiod '106 7 1'

${tools_dir}/grib_set -s eps=0 $temp_interval $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,eps '105 0'
${tools_dir}/grib_set -s eps=1 $temp_instant $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,eps '106 1'
${tools_dir}/grib_set -s stepType=instant $temp_interval $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType '129 instant'
${tools_dir}/grib_set -s stepType=avg $temp_instant $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,stepType,startDateOfReferencePeriod '106 avg 20000101'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=8,paramId=133093,startDateOfReferencePeriod=20000101 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,probabilityType,is_probability_fcst,is_referenceperiod '112 3 1 1'
${tools_dir}/grib_set -s stepType=instant $temp_interval $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType '131 instant'
${tools_dir}/grib_set -s stepType=avg $temp_instant $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,stepType,startDateOfReferencePeriod '112 avg 20000101'

${tools_dir}/grib_set -s tablesVersion=37,productDefinitionTemplateNumber=8,is_referenceperiod=1,is_referenceperiod=1 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,is_referenceperiod '105 1'

# Clean up
rm -f $temp_instant $temp_interval
