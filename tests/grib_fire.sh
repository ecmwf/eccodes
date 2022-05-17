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

label="grib_fire_test"

# Choose a GRIB2 sample with MARS keys
grib2_sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
temp=temp1.${label}.grib2

for pdtn in 70 71 72 73 93 94 97 98; do
  ${tools_dir}/grib_set -s \
    productDefinitionTemplateNumber=$pdtn,marsClass=gw,inputProcessIdentifier=1000,inputOriginatingCentre=98 \
  $grib2_sample $temp
  grib_check_key_equals $temp "datasetForLocal,originatingClass,originOfPostProcessing" "fire 1 ecmf_od"


  ${tools_dir}/grib_set -s \
    productDefinitionTemplateNumber=$pdtn,marsClass=gw,inputProcessIdentifier=23000,inputOriginatingCentre=98 \
  $grib2_sample $temp
  grib_check_key_equals $temp "datasetForLocal,originatingClass,originOfPostProcessing" "fire 23 ecmf_ea"
done

# Clean up
rm -f $temp
