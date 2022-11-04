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

label="grib_keys_iter_test"
tempOut=temp.$label.out

cd ${data_dir}

# ECC-1410
f='tigge/tiggelam_cnmc_sfc.grib'
$EXEC ${test_dir}/grib_keys_iter $f > /dev/null

grib_files=`cat ${data_dir}/grib_data_files.txt`
for f in ${grib_files}; do
    ${test_dir}/grib_keys_iter $f > $tempOut
done

cd ${data_dir}/tigge
tigge_files=`cat ${data_dir}/tigge/tigge_data_files.txt`
for f in ${tigge_files}; do
    ${test_dir}/grib_keys_iter $f > $tempOut
done

rm -f $tempOut
