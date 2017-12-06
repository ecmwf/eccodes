#!/bin/sh

# --- check nothing bombs when changing packing
# ---  for now limited to just a few packing schemes

. ./include.sh

#set -x

grib1=${data_dir}/reduced_latlon_surface_constant.grib1
grib2=${data_dir}/reduced_latlon_surface_constant.grib2

packing1="
	grid_ieee
	grid_simple
	grid_simple_matrix"

packing2="
	grid_ieee
	grid_jpeg
	grid_simple
	grid_simple_log_preprocessing
	grid_simple_matrix"

temp=temp.grib_
rm -f $temp

# --- test changing the packing
# arg 1    : input grib file
# arg 2-n  : packing types to test
function test_packing() {
	grib=$1

	if [ ! -f $grib ]; then
		echo "Input data missing"
		exit 1
	fi

	shift
	while [ $# -gt 0 ]; do
		packing=$1

		${tools_dir}/grib_set -r -s packingType=$packing $grib $temp
		result=`${tools_dir}/grib_get -p packingType $temp`
		
		if [ "$result" != "$packing" ]; then
			echo "Setting packing did not go right"
			exit 1
		fi

		shift
	done

	rm -f $temp
}

test_packing $grib1 $packing1
test_packing $grib2 $packing2
