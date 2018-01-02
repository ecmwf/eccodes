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
REDIRECT=/dev/null

label="grib3_templates"
temp1=temp1.$label.grib3
temp2=temp2.$label.grib3

# Create a sample GRIB3 file converting from existing sample
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s editionNumber=3 $sample_g1 $temp1

# Section 4 templates: Horizontal Domain
# -----------------------------------------
${tools_dir}/grib_set -M -s horizontalDomainTemplateNumber=1 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p angleOfRotation $temp2`
[ "$res" = "0" ]

${tools_dir}/grib_set -M -s horizontalDomainTemplateNumber=2 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p stretchingFactorScaled $temp2`
[ "$res" = "0" ]

${tools_dir}/grib_set -M -s horizontalDomainTemplateNumber=3 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p angleOfRotation,stretchingFactorScaled $temp2`
[ "$res" = "0 0" ]


# Section 5 templates: Vertical Domain
# -----------------------------------------
${tools_dir}/grib_set -M -s verticalDomainTemplateNumber=1 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p level,scaleFactorOfFirstFixedSurface,scaledValueOfFirstFixedSurface $temp2`
[ "$res" = "500 0 50000" ]
res=`${tools_dir}/grib_get -M -p scaleFactorOfFirstFixedSurface,scaleFactorOfSecondFixedSurface $temp2`
[ "$res" = "0 MISSING" ]


# Section 6 templates: Generating Process
# -----------------------------------------
# Try large number of ensembles
${tools_dir}/grib_set -M -s generatingProcessTemplateNumber=1,memberNumber=1000,numberOfMembersInEnsemble=2000 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p memberNumber,numberOfMembersInEnsemble $temp2`
[ "$res" = "1000 2000" ]


# Section 7 templates: Observable Property
# -----------------------------------------
${tools_dir}/grib_set -M -s observablePropertyTemplateNumber=1 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p parameterDiscipline,parameterCategory,parameterNumber $temp2`
[ "$res" = "0 0 0" ] # Temperature
res=`${tools_dir}/grib_get -M -p unitsConversionScaleFactor,unitsConversionOffset $temp2`
[ "$res" = "0 0" ]

${tools_dir}/grib_set -M -s observablePropertyTemplateNumber=2 $temp1 $temp2
res=`${tools_dir}/grib_get -M -p constituentType $temp2`
[ "$res" = "0" ]


rm -f $temp1 $temp2
