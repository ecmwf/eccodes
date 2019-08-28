#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-756
# It tests computing the number of points for Reduced
# Gaussians grids
# ---------------------------------------------------------
label="grib_ecc-756-test"
tempSample=temp.${label}.sample.grib
tempRules=temp.${label}.filter
tempOut=temp.${label}.grib

check_grib1_grid()
{
   t_file=$1
   t_key=$2
   t_expected=$3
   t_result=`${tools_dir}/grib_get -w count=1 -p $t_key $t_file`
   if [ "$t_result" != "$t_expected" ]; then
      echo "File:     $t_file"
      echo "Key(s):   $t_key"
      echo "Expected: $t_expected"
      echo "Result:   $t_result"
      exit 1
   fi
}


check_grib1_grid $data_dir/index.grib                         numberOfDataPoints 213988
check_grib1_grid $data_dir/msl.octa.glob.grib1                numberOfDataPoints 6599680
check_grib1_grid $data_dir/reduced_gaussian_lsm.grib1         numberOfDataPoints 6114
check_grib1_grid $data_dir/reduced_gaussian_model_level.grib1 numberOfDataPoints 6114
check_grib1_grid $data_dir/reduced_gaussian_sub_area.grib1    numberOfDataPoints 53564
check_grib1_grid $data_dir/reference_ensemble_mean.grib1      numberOfDataPoints 213988
check_grib1_grid $data_dir/reference_stdev.grib1              numberOfDataPoints 213988

check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_ml_grib1.tmpl      numberOfDataPoints 6114
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_1024_grib1.tmpl numberOfDataPoints 5447118
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_1280_grib1.tmpl numberOfDataPoints 8505906
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_128_grib1.tmpl  numberOfDataPoints 88838
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_160_grib1.tmpl  numberOfDataPoints 138346
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_2000_grib1.tmpl numberOfDataPoints 20696844
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_200_grib1.tmpl  numberOfDataPoints 213988
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_256_grib1.tmpl  numberOfDataPoints 348528
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_320_grib1.tmpl  numberOfDataPoints 542080
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib1.tmpl   numberOfDataPoints 6114
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_400_grib1.tmpl  numberOfDataPoints 843490
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib1.tmpl   numberOfDataPoints 13280
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_512_grib1.tmpl  numberOfDataPoints 1373624
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_640_grib1.tmpl  numberOfDataPoints 2140702
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_64_grib1.tmpl   numberOfDataPoints 24572
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_80_grib1.tmpl   numberOfDataPoints 35718
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_96_grib1.tmpl   numberOfDataPoints 50662
check_grib1_grid $ECCODES_SAMPLES_PATH/reduced_gg_pl_grib1.tmpl      numberOfDataPoints 6114

rm -f $tempSample $tempRules $tempOut
