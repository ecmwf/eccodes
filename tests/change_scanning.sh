#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
#set -x

editions="1 2"
gridTypes="regular_ll rotated_ll" 
Ni_list="5 8"
Nj_list="7 4"

file=${data_dir}/regular_latlon_surface.grib1

for edition in `echo ${editions}`; do
  for gridType in `echo $gridTypes`; do
    for Ni in `echo ${Ni_list}`; do
      for Nj in `echo ${Nj_list}`; do

      #echo $gridType Ni=$Ni Nj=$Nj
    
      N=`expr $Ni \* $Nj`
      cat > ${data_dir}/change_scanning_${gridType}.filter <<EOF
        set edition=$edition;
        set gridType="$gridType";
        set Ni=$Ni;
        set Nj=$Nj;
    	set latitudeFirstInDegrees=20.0;
    	set longitudeFirstInDegrees=20.0;
    	set latitudeLastInDegrees=`expr 20 - $Nj + 1`;
    	set longitudeLastInDegrees=`expr 20 + $Ni - 1`;
    	set DiInDegrees=1.0;
    	set DjInDegrees=1.0;
    	set iScansNegatively=0;
    	set jScansPositively=0;
    	set jPointsAreConsecutive=0;
    	set values = {1
EOF
      list=""
      i=2
      while [ $i -le $N ]; do
        list="$list,$i"
        i=`expr $i + 1`
      done
    
      cat >> ${data_dir}/change_scanning_${gridType}.filter <<EOF
      $list };
      write "${data_dir}/scan1.grib";
EOF
    
      ${tools_dir}grib_filter ${data_dir}/change_scanning_${gridType}.filter $file
      #ed=`${tools_dir}grib_get -p edition ${data_dir}/scan1.grib`
      #echo edition=$ed
    
      ${tools_dir}grib_set -s swapScanningX=1 ${data_dir}/scan1.grib ${data_dir}/scan.grib
      ${tools_dir}grib_get_data -F "%g" ${data_dir}/scan.grib > ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump
      diff ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}_good.dump
      rm -f ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump
    
      ${tools_dir}grib_set -s swapScanningY=1 ${data_dir}/scan1.grib ${data_dir}/scan.grib
      ${tools_dir}grib_get_data -F "%g" ${data_dir}/scan.grib > ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump
      diff ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}_good.dump 
      rm -f ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump
      rm -f ${data_dir}/change_scanning_${gridType}.filter

      done
    done
  done
done

rm -f ${data_dir}/scan1.grib ${data_dir}/scan.grib

