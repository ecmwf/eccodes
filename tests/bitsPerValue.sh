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

files="
constant_width_bitmap.grib
constant_width_boust_bitmap.grib
gen.grib
gen_bitmap.grib
gen_ext.grib
gen_ext_bitmap.grib
gen_ext_boust.grib
gen_ext_boust_bitmap.grib
gen_ext_spd_2.grib
gen_ext_spd_2_bitmap.grib
gen_ext_spd_2_boust_bitmap.grib
gen_ext_spd_3.grib
gen_ext_spd_3_boust_bitmap.grib
grid_ieee.grib
gts.grib
in_copy.grib
index.grib
mixed.grib
pad.grib
row.grib
satellite.grib
simple.grib
simple_bitmap.grib
sst_globus0083.grib
timeRangeIndicator_0.grib
timeRangeIndicator_10.grib
timeRangeIndicator_5.grib
tp_ecmwf.grib
constant_field.grib1
lfpw.grib1
missing_field.grib1
reduced_gaussian_lsm.grib1
reduced_gaussian_model_level.grib1
reduced_gaussian_pressure_level.grib1
reduced_gaussian_pressure_level_constant.grib1
reduced_gaussian_sub_area.grib1
reduced_gaussian_surface.grib1
reduced_latlon_surface.grib1
reduced_latlon_surface_constant.grib1
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
test.grib1
gep10.t12z.pgrb2af78.grib2
gfs.c255.grib2
jpeg.grib2
lfpw.grib2
missing.grib2
multi.grib2
multi_created.grib2
reduced_gaussian_surface_jpeg.grib2
sample.grib2
test_uuid.grib2
tigge.sm.grib2
tigge.sm.miss.grib2
tigge_af_ecmwf.grib2
tigge_cf_ecmwf.grib2
tigge_ecmwf.grib2
tigge_pf_ecmwf.grib2
v.grib2
"

# Make sure the key "bitsPerValue" is defined for all gribs
# grib_get will fail if the key is not defined
for file in $files; do
  infile=${data_dir}/$file
  if [ -f "$infile" ]; then
    ${tools_dir}grib_get -p bitsPerValue $infile >/dev/null
  fi
done


files="regular_latlon_surface.grib2 \
       regular_latlon_surface.grib1"

for file in `echo $files`; do

  infile=${data_dir}/$file
  outfile1=${infile}_bitsPerValue_1
  outfile2=${infile}_bitsPerValue_2

  ${tools_dir}grib_set -r -s bitsPerValue=10 $infile $outfile1

  ${tools_dir}grib_set -s setBitsPerValue=10 $infile $outfile2

  ${tools_dir}grib_compare $outfile1 $outfile2

  rm -f $outfile1 $outfile2 || true

done

# Extra test for grid_complex_spatial_differencing
res=`${tools_dir}grib_get -p decimalScaleFactor,bitsPerValue ${data_dir}/gfs.c255.grib2`
[ "$res" = "1 20" ]
