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

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempText1=temp.$label.1.txt
tempText2=temp.$label.2.txt

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

files="
reduced_gaussian_model_level.grib2
reduced_gaussian_pressure_level.grib2
reduced_gaussian_pressure_level_constant.grib2
reduced_gaussian_sub_area.grib2
reduced_gaussian_surface.grib2
regular_gaussian_model_level.grib2
regular_gaussian_pressure_level.grib2
regular_gaussian_pressure_level_constant.grib2
regular_gaussian_surface.grib2
regular_latlon_surface.grib2
regular_latlon_surface_constant.grib2
spherical_model_level.grib2
spherical_pressure_level.grib2
tigge_af_ecmwf.grib2
mixed.grib
lfpw.grib1
test_file.grib2
"

# Check if JPEG is enabled
if [ $HAVE_JPEG -eq 1 ]; then
    # Include files which have messages with grid_jpeg packing
    echo "Add extra files (HAVE_JPEG=1)"
    files="reduced_gaussian_surface_jpeg.grib2 v.grib2 "$files
fi

for file in $files; do
    input=${data_dir}/$file
    # -f => FULL
    # -p => PARTIAL
    $EXEC ${test_dir}/grib_partial_geometry -f $input > $tempText1
    $EXEC ${test_dir}/grib_partial_geometry -p $input > $tempText2
    diff $tempText1 $tempText2
done

# Cleanup
rm -f $tempText1 $tempText2
