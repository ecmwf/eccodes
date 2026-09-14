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

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Example concept entry with both keys exists for tablesVersion=37:
# paramId=133093

# Instantaneous case -> PDT 4.131
${tools_dir}/grib_set -s tablesVersion=37,paramId=133093 $sample_grib2 $temp_instant
grib_check_key_equals $temp_instant productDefinitionTemplateNumber,stepType,probabilityType,typeOfRelationToReferenceDataset '131 instant 3 1'

# Time-interval case -> PDT 4.112
${tools_dir}/grib_set -s tablesVersion=37,stepType=avg,paramId=133093 $sample_grib2 $temp_interval
grib_check_key_equals $temp_interval productDefinitionTemplateNumber,probabilityType,typeOfRelationToReferenceDataset '112 3 1'

# Clean up
rm -f $temp_instant $temp_interval
