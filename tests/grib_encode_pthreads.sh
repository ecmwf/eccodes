#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

input=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl

${test_dir}/grib_encode_pthreads $input

rm -f temp.grib_encode_pthreads.out_*.grib
