#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

rm -f log | true
workdir=`pwd`

cd ${data_dir}
infile=regular_gaussian_model_level.grib1

${tools_dir}grib_ls -P count $infile > log
${tools_dir}grib_ls -p count,step $infile >> log
${tools_dir}grib_ls $infile >> log
${tools_dir}grib_ls -l 0,0,1 $infile >> log
${tools_dir}grib_get -l 0,0,1 $infile >> log
${tools_dir}grib_get -p count,step $infile >> log
${tools_dir}grib_get -P count $infile >> log

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
	echo $file >> log
	${tools_dir}grib_ls -l 40,28 $file  | grep index | awk '{print $4;}' >> log
done

diff log ls.log 
rm -f log

cd $workdir
