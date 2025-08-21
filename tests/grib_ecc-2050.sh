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
# This is the test for JIRA issue ECC-2050
# GRIB: Modifying section 2 unexpectedly alters section 4 PDT
# ---------------------------------------------------------

label="grib_ecc-2050_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempLog=temp.$label.log

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s setLocalDefinition=1,productDefinitionTemplateNumber=11,typeOfStatisticalProcessing=missing $sample $tempGribA
${tools_dir}/grib_set -s localDefinitionNumber=1 $tempGribA $tempGribB

grib_check_key_equals $tempGribA stepType instant
grib_check_key_equals $tempGribB productDefinitionTemplateNumber 11


# Clean up
rm -f $tempGribA $tempGribB $tempLog
