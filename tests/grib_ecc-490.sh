#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-490.
# It tests changing GRIB templates
# ---------------------------------------------------------
label="grib_ecc-490-test"
tempSample=temp.${label}.sample.grib
tempRules=temp.${label}.filter
tempOut=temp.${label}.grib

grib2_sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_128_grib2.tmpl
${tools_dir}/grib_set -s productDefinitionTemplateNumber=11,typeOfProcessedData=pf,typeOfLevel=surface $grib2_sample $tempSample

cat > $tempRules <<EOF
    set  productDefinitionTemplateNumber=1;
    set  typeOfFirstFixedSurface=1;
    set  typeOfSecondFixedSurface=255;
    set  typeOfFirstFixedSurface=100;
    set  productDefinitionTemplateNumber=11;
    set  typeOfFirstFixedSurface=100;
    set  typeOfSecondFixedSurface=255;
    write;
EOF

${tools_dir}/grib_filter -o $tempOut $tempRules $tempSample

rm -f $tempSample $tempRules $tempOut
