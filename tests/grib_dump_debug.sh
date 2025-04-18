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
label="grib_dump_debug_test"
temp=temp.$label.txt
temp2=temp.2.$label.txt

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    unset ECCODES_SAMPLES_PATH
fi

files="
constant_field.grib1
lfpw.grib1
missing_field.grib1
reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level_constant.grib1
reduced_gaussian_sub_area.grib1
reduced_gaussian_surface.grib1
reference_ensemble_mean.grib1
reference_stdev.grib1
regular_gaussian_model_level.grib1
regular_gaussian_pressure_level.grib1
regular_gaussian_pressure_level_constant.grib1
regular_gaussian_surface.grib1
regular_latlon_surface.grib1
regular_latlon_surface_constant.grib1
second_ord_rbr.grib1
small_ensemble.grib1
spectral_complex.grib1
spherical_model_level.grib1
spherical_pressure_level.grib1
constant_field.grib2
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
tigge_cf_ecmwf.grib2
"

# Check if JPEG is enabled
if [ $HAVE_JPEG -eq 1 ]; then
    # Include files which have messages with grid_jpeg packing
    echo "Add extra files (HAVE_JPEG=1)"
    files="reduced_gaussian_surface_jpeg.grib2 "$files
fi

for file in $files; do
   if [ -f ${data_dir}/$file ]; then
      ${tools_dir}/grib_dump -w count=1 -Da ${data_dir}/$file > $temp 2>&1
      set +e
      # Look for the word ERROR in output. We should not find any
      grep -q 'ERROR ' $temp
      if [ $? -eq 0 ]; then
         echo "File $file: found string ERROR in grib_dump output! See file $temp"
         exit 1
      fi
      set -e
   fi
done

# These files have some invalid surface keys which the message validity checker should pick up
if [ $HAVE_JPEG -eq 1 ]; then
    files="jpeg.grib2 v.grib2 multi.grib2 gfs.c255.grib2 missing.grib2 test_uuid.grib2"
    for file in $files; do
        ${tools_dir}/grib_dump -w count=1 -Da ${data_dir}/$file > $temp 2>&1
        set +e
        grep -q 'ERROR ' $temp | grep -v "Message Validity Checks" > $temp2
        set -e
        if [ -s "$temp2" ]; then
            # File exists and has a size greater than zero
            echo "File $file: Check contents of $temp2"
            exit 1
        fi
    done
fi

# ECC-1247: indicate which keys can have values which are 'missing'
# ECC-1584: indicate which keys are 'read-only'
# ------------------------------------------------------------------
infile=${data_dir}/sample.grib2
${tools_dir}/grib_dump -D $infile > $temp
grep -q "unsigned hoursAfterDataCutoff = 0 (can be missing)" $temp
grep -q "unsigned iDirectionIncrement = 2000000 (can be missing)" $temp
grep -q "constant zero = 0 (read-only)" $temp
grep -q "unsigned reserved = MISSING (can be missing) (read-only)" $temp

# Debug dump a BUFR using -TB switch
infile=${data_dir}/bufr/pgps_110.bufr
${tools_dir}/grib_dump -Da -TB $infile > $temp
# ${tools_dir}/grib_dump -Da -TB -s unpack=1 $infile > $temp


# Clean up
rm -f $temp $temp2
