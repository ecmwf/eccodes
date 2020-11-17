#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
label="grib_cfNames_test"
temp=temp.${label}
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s discipline=192,parameterCategory=128,parameterNumber=137 $sample2 $temp
grib_check_key_equals $temp cfName lwe_thickness_of_atmosphere_mass_content_of_water_vapor

${tools_dir}/grib_set -s discipline=0,parameterNumber=20,parameterCategory=2 $sample2 $temp
grib_check_key_equals $temp cfName kinetic_energy_dissipation_in_atmosphere_boundary_layer
${tools_dir}/grib_set -s paramId=145 $sample2 $temp
grib_check_key_equals $temp cfName kinetic_energy_dissipation_in_atmosphere_boundary_layer
${tools_dir}/grib_set -s paramId=137 $sample2 $temp
grib_check_key_equals $temp cfName lwe_thickness_of_atmosphere_mass_content_of_water_vapor

${tools_dir}/grib_set -s indicatorOfParameter=137,table2Version=128 $sample1 $temp
grib_check_key_equals $temp cfName lwe_thickness_of_atmosphere_mass_content_of_water_vapor

${tools_dir}/grib_set -s paramId=145 $sample2 $temp
grib_check_key_equals $temp cfName kinetic_energy_dissipation_in_atmosphere_boundary_layer

${tools_dir}/grib_set -s indicatorOfParameter=40,table2Version=3 $sample1 $temp
grib_check_key_equals $temp cfName lagrangian_tendency_of_air_pressure

${tools_dir}/grib_set -s paramId=151154 $sample1 $temp
grib_check_key_equals $temp cfName surface_downward_northward_stress
${tools_dir}/grib_set -s paramId=151154 $sample2 $temp
grib_check_key_equals $temp cfName surface_downward_northward_stress

# Clean up
rm -f $temp
