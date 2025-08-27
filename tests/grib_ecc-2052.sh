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
# This is the test for JIRA issue ECC-2052
# GRIB2: setting stream to enda changes the PDTN
# ---------------------------------------------------------

label="grib_ecc-2052_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s \
  setLocalDefinition=1,grib2LocalSectionNumber=36,class=40,type=9,stream=1249,productDefinitionTemplateNumber=143 \
  $sample_grib2 $tempGribA

${tools_dir}/grib_set -s stream=1030 $tempGribA $tempGribB
grib_check_key_equals $tempGribB productDefinitionTemplateNumber,randomFieldNumber '143 0'


# Clean up
rm -f $tempGribA $tempGribB $tempFilt $tempLog $tempOut $tempRef
