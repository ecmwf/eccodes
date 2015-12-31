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

workdir=`pwd`
REDIRECT=/dev/null

cd ${data_dir}

encoding=1
simple_no_bitmap=simple.grib
simple_bitmap=simple_bitmap.grib

files_no_bitmap="gen_ext.grib \
gen_ext_boust.grib \
gen_ext_spd_2.grib \
gen_ext_spd_3.grib \
gen.grib row.grib "

files_bitmap="constant_width_bitmap.grib \
constant_width_boust_bitmap.grib \
gen_bitmap.grib \
gen_ext_bitmap.grib \
gen_ext_boust_bitmap.grib \
gen_ext_spd_2_bitmap.grib \
gen_ext_spd_2_boust_bitmap.grib \
gen_ext_spd_3_boust_bitmap.grib \
simple_bitmap.grib"

no_packing="gen.grib|row.grib|gen_bitmap.grib|constant_width_bitmap.grib|constant_width_boust_bitmap.grib"

test_data() {
	${tools_dir}grib_filter test.filter $simple > $simple.data

	for f in $files
	do 
		${tools_dir}grib_filter test.filter $f > $f.data
		diff $simple.data $f.data > /dev/null
		${tools_dir}grib_compare -cvalues $f $simple
		echo $f decoding test passed > $REDIRECT

		exclude=`echo $f | awk " /$no_packing/ {print \"found\";} "`
		if [ -z "$exclude" ] && [ $encoding != 0 ]
		then
			rm -f $f.copied
			${tools_dir}grib_copy -r $f $f.copied
			${tools_dir}grib_filter test.filter $f.copied > $f.copied.data
			diff $simple.data $f.copied.data > /dev/null
			${tools_dir}grib_compare -cvalues $f.copied $simple
			echo $f encoding test passed > $REDIRECT
			echo > $REDIRECT
		fi

	done
	rm -f *.data *.copied
}


cat > test.filter<<EOF
print "[values!1]!";
EOF

simple=$simple_no_bitmap
files=$files_no_bitmap
test_data

simple=$simple_bitmap
files=$files_bitmap
test_data

# Now make sure grib_dump works on a second-order row-by-row file
# See GRIB-147
${tools_dir}grib_dump second_ord_rbr.grib1 > $REDIRECT

# Test nearest neighbour on second order with a bitmap
# GRIB-541
sec_ord_bmp=sec_ord_bmp.$$.grib1

# Convert to second order packing
${tools_dir}grib_set -r -s packingType=grid_second_order gen_bitmap.grib $sec_ord_bmp
# Check there are missing values
nums=`${tools_dir}grib_get -p numberOfDataPoints,numberOfCodedValues,numberOfMissing $sec_ord_bmp`
[ "$nums" = "5969 4 5965" ]

res=`${tools_dir}grib_get -l 33,88.5 $sec_ord_bmp`
[ "$res" = "9999 5.51552 9999 9999 " ]

res=`${tools_dir}/grib_get -l 28.5,90 $sec_ord_bmp`
[ "$res" = "3.51552 9999 5.26552 9999 " ]

# GRIB-203 nearest on M-F second order boustrophedonic
res=`${tools_dir}grib_get -w count=1 -l 0,0 lfpw.grib1`
[ "$res" = "20560.7 20563.4 20554.7 20559.5 " ]

rm -f $sec_ord_bmp
rm -f test.filter
