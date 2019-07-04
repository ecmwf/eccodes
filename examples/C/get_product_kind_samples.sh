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

label="get_product_kind_samples_c"
fTmp=${label}.tmp

sample_grib1=${ECCODES_SAMPLES_PATH}/GRIB1.tmpl
${examples_dir}/c_get_product_kind $sample_grib1 > $fTmp
grep -q 'product: GRIB' $fTmp

sample_bufr4=${ECCODES_SAMPLES_PATH}/BUFR4.tmpl
${examples_dir}/c_get_product_kind $sample_bufr4 > $fTmp
grep -q 'product: BUFR' $fTmp

rm -f $fTmp
