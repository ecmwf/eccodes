#!/bin/sh

# check the fortran against the tools for the support for 
# multi gribs to be consistent

#set -x

. ./include.sh

files="regular_latlon_surface_constant.grib1 \
	regular_latlon_surface_constant.grib2"

# check against tool with support for multi off
function test_multi() {
	for grib_file in `echo $files`; do
		grib_file=${data_dir}/$grib_file

		if [ ! -f $grib_file ]; then
			echo "Data file missing: $grib_file"
			exit 1
		fi

		fn=`${examples_dir}/$1 $grib_file`
		n=`${tools_dir}/$2 -p count $grib_file | wc -l`
		if [ ! $fn = $n ]; then
			false
			exit
		fi
	done
}

test_multi "message_count_multi 0" "grib_get -M"
test_multi "message_count_multi 1" "grib_get"
