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

label="grib2_datasetforlocal_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempGribC=temp.$label.C.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

# ERA6
# -----
sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
# Make changes in stages
${tools_dir}/grib_set -s marsClass=e6,productDefinitionTemplateNumber=42 $sample $tempGribA
${tools_dir}/grib_set -s paramId=233033 $tempGribA $tempGribB

# Now all in one go
${tools_dir}/grib_set -s marsClass=e6,productDefinitionTemplateNumber=42,paramId=233033 $sample $tempGribC
cmp $tempGribB $tempGribC

# DestinE
# --------
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s centre=0,productionStatusOfProcessedData=12 $sample $tempGribA
${tools_dir}/grib_set -s paramId=210061 $tempGribA $tempGribB

${tools_dir}/grib_set -s centre=0,productionStatusOfProcessedData=12,paramId=210061 $sample $tempGribC
cmp $tempGribB $tempGribC

#grib_check_key_equals $temp k1,k2 "v1 v2"

# Clean up
rm -f $tempGribA $tempGribB $tempGribC $tempFilt $tempLog $tempOut $tempRef
