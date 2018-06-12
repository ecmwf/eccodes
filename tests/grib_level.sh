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
#set -x

sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
temp=temp.level.grib
temp2=temp2.level.grib

file=${data_dir}/regular_gaussian_pressure_level.grib1

cat >temp.level.good<<EOF
grib1: level=850 isobaricInhPa indicatorOfTypeOfLevel=100
grib2: level=850 isobaricInhPa pl 85000
grib1: level=850 isobaricInhPa indicatorOfTypeOfLevel=100
grib1: level=50 isobaricInPa indicatorOfTypeOfLevel=210
grib2: level=50 isobaricInPa pl 50
grib1: level=50 isobaricInPa indicatorOfTypeOfLevel=210
grib1: level=1 topLevel=1 bottomLevel=2 depthBelowLandLayer indicatorOfTypeOfLevel=112
grib2: level=1 topLevel=1 bottomLevel=2 depthBelowLandLayer first= sfc 1 second= sfc 2
grib1: level=1 topLevel=1 bottomLevel=2 depthBelowLandLayer indicatorOfTypeOfLevel=112
EOF

cat >level.filter<<EOF
set typeOfLevel="isobaricInhPa";
set level=850;
print "grib[edition]: level=[level] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
set edition=2;
print "grib[edition]: level=[level] [typeOfLevel] [typeOfFirstFixedSurface:s] [scaledValueOfFirstFixedSurface]";
set edition=1;
print "grib[edition]: level=[level] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
set edition=1;
set typeOfLevel="isobaricInPa";
set level=50;
print "grib[edition]: level=[level] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
set edition=2;
print "grib[edition]: level=[level] [typeOfLevel] [typeOfFirstFixedSurface:s] [scaledValueOfFirstFixedSurface]";
set edition=1;
print "grib[edition]: level=[level] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
set typeOfLevel="depthBelowLandLayer";
set topLevel=1;
set bottomLevel=2;
print "grib[edition]: level=[level] topLevel=[topLevel] bottomLevel=[bottomLevel] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
set edition=2;
print "grib[edition]: level=[level] topLevel=[topLevel] bottomLevel=[bottomLevel] [typeOfLevel] first= [typeOfFirstFixedSurface:s] [scaledValueOfFirstFixedSurface] second= [typeOfSecondFixedSurface:s] [scaledValueOfSecondFixedSurface]";
set edition=1;
print "grib[edition]: level=[level] topLevel=[topLevel] bottomLevel=[bottomLevel] [typeOfLevel] indicatorOfTypeOfLevel=[indicatorOfTypeOfLevel:l]";
EOF

    
${tools_dir}/grib_filter level.filter $file > test.dump
diff temp.level.good test.dump

# Use of 'level' key for GRIB2
${tools_dir}/grib_set -s typeOfFirstFixedSurface=103,level=24 $sample_g2 $temp
grib_check_key_equals $temp level,scaledValueOfFirstFixedSurface,scaleFactorOfFirstFixedSurface '24 24 0'
${tools_dir}/grib_set -s typeOfFirstFixedSurface=103,level=2.4 $sample_g2 $temp
grib_check_key_equals $temp level:d,scaledValueOfFirstFixedSurface,scaleFactorOfFirstFixedSurface '2.4 240 2'

# Use a parameter which has two levels
${tools_dir}/grib_set -s paramId=228086,topLevel=1.3,bottomLevel=5.4 $sample_g2 $temp
grib_check_key_equals $temp 'topLevel:d,bottomLevel:d' '1.3 5.4'
grib_check_key_equals $temp scaleFactorOfFirstFixedSurface,scaledValueOfFirstFixedSurface '2 130'
grib_check_key_equals $temp scaleFactorOfSecondFixedSurface,scaledValueOfSecondFixedSurface '2 540'

# GRIB-492
${tools_dir}/grib_set -s indicatorOfTypeOfLevel=110 $sample_g1 $temp
res=`${tools_dir}/grib_get -p indicatorOfTypeOfLevel:l,topLevel,bottomLevel $temp`
[ "$res" = "110 0 0" ]

# GRIB-415 evaluate level as a double
${tools_dir}/grib_set -s scaledValueOfFirstFixedSurface=15,scaleFactorOfFirstFixedSurface=1 $sample_g2 $temp
res=`${tools_dir}/grib_get -p level:d $temp`
[ "$res" = "1.5" ]

# GRIB-637 grib2 Potential vorticity surface
input=${data_dir}/tigge_pf_ecmwf.grib2
res=`${tools_dir}/grib_get -wcount=7 -F%.20f -p level:d $input`
[ "$res" = "2.00000000000000000000" ]

# Setting productDefinitionTemplateNumber should keep level info
${tools_dir}/grib_set -s typeOfFirstFixedSurface=100,level=5 $sample_g2 $temp
${tools_dir}/grib_set -s productDefinitionTemplateNumber=1 $temp $temp2
grib_check_key_equals $temp2 level 5

# ECC-530: Setting typeOfSecondFixedSurface should not overwrite
# scale factor and scaled value of first fixed surface
# cat >level.filter<<EOF
#   set typeOfFirstFixedSurface=106;
#   set scaleFactorOfFirstFixedSurface=-2;
#   set scaledValueOfFirstFixedSurface=4;
#   set typeOfSecondFixedSurface=106; # Should not overwrite
#   assert(scaledValueOfFirstFixedSurface == 4);
#   assert(scaleFactorOfFirstFixedSurface == -2);
#   assert(level==400);
# EOF
# ${tools_dir}/grib_filter level.filter $sample_g2

# Related to ECC-530: make sure GRIB2 parameters with typeOfSecondFixedSurface
# have correct parameter definitions (grib_set does not cause scale factor/value loss
params="228086 228087 228095 228096 228170 228171 228039 228139"
for pid in $params; do
 ${tools_dir}/grib_set -s paramId=$pid $sample_g2 $temp
 grib_check_key_equals $temp paramId $pid
done


rm -f level.filter temp.level.good test.dump $temp $temp2
