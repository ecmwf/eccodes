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

tempLog=temp.ls.log
rm -f $tempLog

cd ${data_dir}
infile=regular_gaussian_model_level.grib1

${tools_dir}/grib_ls -P count $infile       >  $tempLog
${tools_dir}/grib_ls -p count,step $infile  >> $tempLog
${tools_dir}/grib_ls $infile                >> $tempLog
${tools_dir}/grib_ls -l 0,0,1 $infile       >> $tempLog
${tools_dir}/grib_get -l 0,0,1 $infile      >> $tempLog
${tools_dir}/grib_get -p count,step $infile >> $tempLog
${tools_dir}/grib_get -P count $infile      >> $tempLog

files=" reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_model_level.grib2
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level.grib2
reduced_gaussian_pressure_level_constant.grib1
reduced_gaussian_pressure_level_constant.grib2
reduced_gaussian_sub_area.grib1
reduced_gaussian_sub_area.grib2
reduced_gaussian_surface.grib1
reduced_gaussian_surface.grib2
reduced_latlon_surface.grib1
reduced_latlon_surface.grib2
regular_gaussian_model_level.grib1
regular_gaussian_model_level.grib2
regular_gaussian_pressure_level.grib1
regular_gaussian_pressure_level.grib2
regular_gaussian_pressure_level_constant.grib1
regular_gaussian_pressure_level_constant.grib2
regular_gaussian_surface.grib1
regular_gaussian_surface.grib2
regular_latlon_surface.grib1
regular_latlon_surface.grib2
"

for file in $files
do
  [ -f "$file" ]
  echo $file >> $tempLog
  ${tools_dir}/grib_ls -l 40,28 $file  | grep index | awk '{print $4;}' >> $tempLog
done

diff $tempLog ls.log 
rm -f $tempLog

# Test for bug GRIB-56
${tools_dir}/grib_set -s typeOfLevel=depthBelowLandLayer,topLevel=missing regular_latlon_surface.grib1 tmp_rlls.grib1
${tools_dir}/grib_ls -plevels tmp_rlls.grib1 | grep MISSING>/dev/null

${tools_dir}/grib_set -s typeOfLevel=depthBelowLandLayer,bottomLevel=missing regular_latlon_surface.grib1 tmp_rlls.grib1
${tools_dir}/grib_ls -plevels tmp_rlls.grib1 | grep MISSING>/dev/null
rm -f tmp_rlls.grib1

# GRIB-305. GRIB edition 1 file with one large message
if [ -f "sst_globus0083.grib" ]; then
   ${tools_dir}/grib_ls sst_globus0083.grib > /dev/null
fi

# GRIB-387 printing key of type byte
${tools_dir}/grib_ls -p uuidOfVGrid test_uuid.grib2 > /dev/null

type=`${tools_dir}/grib_get -wcount=1 -p typeOfLevel test_uuid.grib2`
[ "$type" = "generalVertical" ]

# GRIB-213 nearest with land-sea mask
temp_ls=test.grib-213.temp
${tools_dir}/grib_ls -l 85,13,1,reduced_gaussian_lsm.grib1 reduced_gaussian_surface.grib1 >$temp_ls
grep -q 'Point chosen #3 index=21 .* distance=11\.' $temp_ls

${tools_dir}/grib_ls -l 53,2,1,reduced_gaussian_lsm.grib1 reduced_gaussian_surface.grib1 >$temp_ls
grep -q 'Point chosen #2 index=749 .* distance=204\.' $temp_ls

rm -f $temp_ls

