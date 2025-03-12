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

label="grib2_probability_test"
temp=temp.$label
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempSample=temp.$label.tmpl
tempRef=temp.$label.ref
tempOut=temp.$label.txt

sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`

# Automatic PDT selection
# -----------------------
${tools_dir}/grib_set -s tablesVersion=$latest,paramId=131256 $sample2 $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber,shortName,probabilityType '5 10cogug25 3'

# Clean up
rm -f $tempSample $temp $tempGribA $tempGribB $tempRef $tempOut
