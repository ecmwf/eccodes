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

label="grib_change_scanning_test"

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
    
      ${tools_dir}/grib_filter ${data_dir}/change_scanning_${gridType}.filter $file
      #ed=`${tools_dir}/grib_get -p edition ${data_dir}/scan1.grib`
      #echo edition=$ed
    
      ${tools_dir}/grib_set -s swapScanningX=1 ${data_dir}/scan1.grib ${data_dir}/scan.grib
      ${tools_dir}/grib_get_data -F "%g" ${data_dir}/scan.grib > ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump
      diff ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}_good.dump
      rm -f ${data_dir}/scan_x_${gridType}_${Ni}_${Nj}.dump
    
      ${tools_dir}/grib_set -s swapScanningY=1 ${data_dir}/scan1.grib ${data_dir}/scan.grib
      ${tools_dir}/grib_get_data -F "%g" ${data_dir}/scan.grib > ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump
      diff ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}_good.dump 
      rm -f ${data_dir}/scan_y_${gridType}_${Ni}_${Nj}.dump
      rm -f ${data_dir}/change_scanning_${gridType}.filter

      done
    done
  done
done

# alternativeRowScanning
# -----------------------
tempFilt=temp.$label.filt
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempText=temp.$label.txt
tempRef=temp.$label.ref

cat > $tempFilt <<EOF
  set Nj = 2;
  set latitudeOfFirstGridPointInDegrees = 60;
  set latitudeOfLastGridPointInDegrees  = 59;
  set Ni = 3;
  set longitudeOfFirstGridPointInDegrees = 0;
  set longitudeOfLastGridPointInDegrees  = 2;
  set DjInDegrees = 1;
  set DiInDegrees = 1;

  set alternativeRowScanning = 1;
  set values={ 12, 13, 14, 15, 16, 17 };
  write;
EOF

${tools_dir}/grib_filter -o $tempGribA $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s swapScanningAlternativeRows=1 $tempGribA $tempGribB
grib_check_key_equals $tempGribB alternativeRowScanning 0

${tools_dir}/grib_get_data -F "%.2f" $tempGribB > $tempText
cat > $tempRef << EOF
Latitude Longitude Value
   60.000    0.000 12.00
   60.000    1.000 13.00
   60.000    2.000 14.00
   59.000    0.000 17.00
   59.000    1.000 16.00
   59.000    2.000 15.00
EOF
diff $tempRef $tempText

# It must fail when Ni=missing (reduced grids)
set +e
${tools_dir}/grib_set -s swapScanningAlternativeRows=1 $ECCODES_SAMPLES_PATH/reduced_gg_pl_96_grib2.tmpl $tempGribB 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]

# Invalid Ni/Nj
${tools_dir}/grib_set -s Ni=9 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGribA
set +e
${tools_dir}/grib_set -s swapScanningAlternativeRows=1 $tempGribA $tempGribB
status=$?
set -e
[ $status -ne 0 ]

# ECC-1492
grib_check_key_equals "$ECCODES_SAMPLES_PATH/GRIB1.tmpl" iScansNegatively,iScansPositively '0 1'
grib_check_key_equals "$ECCODES_SAMPLES_PATH/GRIB1.tmpl" jScansNegatively,jScansPositively '1 0'

grib_check_key_equals "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" iScansNegatively,iScansPositively '0 1'
grib_check_key_equals "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" jScansNegatively,jScansPositively '1 0'

# Rare case of pack_string for an integer key
${tools_dir}/grib_set -s swapScanningX:s=1 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGribA 2>/dev/null
${tools_dir}/grib_set -s swapScanningX:i=1 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGribB 2>/dev/null
cmp $tempGribA $tempGribB

# Clean up
rm -f $tempFilt $tempGribA $tempGribB $tempRef $tempText
rm -f ${data_dir}/scan1.grib ${data_dir}/scan.grib
