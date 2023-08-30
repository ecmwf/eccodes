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

label="grib_iterator_test"
tempText=temp.$label.txt
tempGrib=temp.$label.grib

files="reduced_latlon_surface.grib1 \
      reduced_gaussian_pressure_level.grib1 \
      regular_gaussian_pressure_level.grib1 \
      reduced_gaussian_sub_area.grib1 \
      regular_latlon_surface.grib1 \
      reduced_latlon_surface.grib2  \
      reduced_gaussian_pressure_level.grib2 \
      regular_gaussian_pressure_level.grib2 \
      reduced_gaussian_sub_area.grib2 \
      regular_latlon_surface.grib2"

for f in $files; do
 file=${data_dir}/$f
 # Must exclude the first line of grib_get_data which is "Latitude Longitude Value"
 iterator_count=`${tools_dir}/grib_get_data -m 9999:missing -f -p centre -F "%g" -w count=1 $file | grep -v Lat |wc -l `
 numberOfPoints=`${tools_dir}/grib_get -w count=1 -p numberOfPoints $file`
 [ $numberOfPoints = ${iterator_count} ]
done


# ECC-822: Increase lat/lon decimals using default grib_get_data
${tools_dir}/grib_get_data -L%12.6f%11.5f ${data_dir}/regular_latlon_surface.grib2 > $tempText

${tools_dir}/grib_get_data -p shortName,level ${data_dir}/regular_latlon_surface.grib2 > $tempText
grep -q "Latitude Longitude Value shortName level" $tempText


# Run on a spectral field - should just print out its values
${tools_dir}/grib_get_data "$samp_dir/sh_ml_grib2.tmpl" > $tempText


# ECC-1642: badly encoded regular grids
# -------------------------------------
${tools_dir}/grib_set -s Ni=33 $samp_dir/GRIB2.tmpl $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set +e
[ $status -ne 0 ]
grep -q "Grid description is wrong or inconsistent" $tempText


${tools_dir}/grib_set -s Ni=MISSING $samp_dir/GRIB2.tmpl $tempGrib
set +e
${tools_dir}/grib_get_data $tempGrib > $tempText 2>&1
status=$?
set +e
[ $status -ne 0 ]
grep -q "Grid description is wrong or inconsistent" $tempText


# Clean up
rm -f $tempText $tempGrib
