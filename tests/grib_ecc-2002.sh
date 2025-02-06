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

label="grib_ecc-2002_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempOut=temp.$label.txt
tempRef=temp.$label.ref

# dataset unknown
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
grib_check_key_equals $sample_grib2 conceptsDir1 "grib2/localConcepts/[centre:s]"
grib_check_key_equals $sample_grib2 conceptsDir2 "grib2"

# DestinE
${tools_dir}/grib_set -s productionStatusOfProcessedData=12 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib conceptsDir1 "grib2"
grib_check_key_equals $tempGrib conceptsDir2 "grib2/localConcepts/destine"

# ERA datasets
${tools_dir}/grib_set -s setLocalDefinition=1,marsClass=e6,productDefinitionTemplateNumber=42,paramId=228080 \
   $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib paramId,shortName "228080 aco2nee"
grib_check_key_equals $tempGrib conceptsDir1 "grib2"
grib_check_key_equals $tempGrib conceptsDir2 "grib2/localConcepts/era6"

${tools_dir}/grib_set -s setLocalDefinition=1,marsClass=e2,productDefinitionTemplateNumber=42,paramId=228080 \
   $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib paramId,shortName "228080 aco2nee"
grib_check_key_equals $tempGrib conceptsDir1 "grib2"
grib_check_key_equals $tempGrib conceptsDir2 "grib2/localConcepts/era"

# DestinE: Make sure the ECMWF parameter is not selected
${tools_dir}/grib_set -s centre=0,productionStatusOfProcessedData=12,paramId=210061 $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib paramId,shortName "210061 co2"


# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef
