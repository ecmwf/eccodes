#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.


. ./include.ctest.sh

label="grib_md5_test"
temp=temp.$label.grib

test_md5()
{
    file=$ECCODES_SAMPLES_PATH/$1
    expected=$2

    result=`${tools_dir}/grib_get -p md5GridSection:s $file`
    [ "$result" = "$expected" ]
}

# Check the md5GridSection for some sample files
# ------------------------------------------------
# GRIB edition 1
test_md5 "sh_sfc_grib1.tmpl"        "5b68b3d2c6e834fd501a123391b6b8a2"
test_md5 "sh_pl_grib1.tmpl"         "5b68b3d2c6e834fd501a123391b6b8a2"
test_md5 "gg_sfc_grib1.tmpl"        "97451fbe92a77bdeb4582fadc9b65e9d"

# GRIB edition 2
test_md5 "sh_sfc_grib2.tmpl"        "b10d32ebf4f91e52df0fc5d1deb57c3d"
test_md5 "rotated_ll_pl_grib2.tmpl" "775bd4622d6becca2d2da9e308c97a7d"
test_md5 "gg_sfc_grib2.tmpl"        "a63333bd3cb80563e66907bb66d82cda"

# Change some keys in the Grid Section and test md5 is different
# ------------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s swapScanningX=1 $input $temp
md1=`${tools_dir}/grib_get -p md5GridSection:s $input`
md2=`${tools_dir}/grib_get -p md5GridSection:s $temp`
[ "$md1" != "$md2" ]

# Clean up
rm -f $temp
