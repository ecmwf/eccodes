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

label="grib_ecc-1313_test"
tempOut=temp.$label.text
tempRef=temp.$label.ref
tempFilt=temp.$label.filt
tempGrb=temp.$label.grib

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempFilt << EOF
 set centre = 34;     # Japan
 print "parameterName             = |[parameterName]|";
 print "centreDescription         = |[centreDescription]|";
 print "gridDefinitionDescription = |[gridDefinitionDescription]|";
 print "nameOfFirstFixedSurface   = |[nameOfFirstFixedSurface]|";
EOF

${tools_dir}/grib_filter $tempFilt $sample_grib2 > $tempOut
cat $tempOut

cat > $tempRef << EOF
parameterName             = |Temperature|
centreDescription         = |Japanese Meteorological Agency - Tokyo|
gridDefinitionDescription = |Latitude/longitude|
nameOfFirstFixedSurface   = |Ground or water surface|
EOF

diff $tempRef $tempOut

# GRIB2: Chemicals and Probability templates
${tools_dir}/grib_set -s tablesVersion=28,productDefinitionTemplateNumber=40,constituentType=10009 $sample_grib2 $tempGrb
grib_check_key_equals $tempGrb constituentTypeName 'Ethene'

${tools_dir}/grib_set -s tablesVersion=28,productDefinitionTemplateNumber=5,probabilityType=2 $sample_grib2 $tempGrb
grib_check_key_equals $tempGrb probabilityTypeName 'Probability of event between lower and upper limits'


rm -f $tempOut $tempRef $tempFilt $tempGrb
