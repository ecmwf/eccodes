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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-2137
# Cannot convert spectral_simple to spectral_complex
# ---------------------------------------------------------

label="grib_ecc-2137_test"
tempGribSimple=temp.$label.simple.grib
tempGribComplex=temp.$label.complex.grib
tempFilt=temp.$label.filt
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

${tools_dir}/grib_set -r -s packingType=spectral_simple  $sample_grib2   $tempGribSimple
${tools_dir}/grib_set -r -s packingType=spectral_complex $tempGribSimple $tempGribComplex

grib_check_key_equals $tempGribSimple packingType "spectral_simple"


stats0=`${tools_dir}/grib_get -F%.2f -p avg,enorm,sd $sample_grib2`
stats1=`${tools_dir}/grib_get -F%.2f -p avg,enorm,sd $tempGribSimple`
stats2=`${tools_dir}/grib_get -F%.2f -p avg,enorm,sd $tempGribComplex`
[ "$stats0" = "$stats1" ]
[ "$stats1" = "$stats2" ]


${tools_dir}/grib_ls -jn statistics $sample_grib2 $tempGribSimple $tempGribComplex

#${tools_dir}/grib_compare $sample_grib2 $tempGribComplex


# Clean up
rm -f $tempGribSimple $tempGribComplex $tempFilt $tempRef
