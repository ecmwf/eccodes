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
    grid_simple
    grid_simple_matrix"
#TODO: grid_simple_log_preprocessing

if [ $HAVE_JPEG -eq 1 ]; then
    packing2="grid_jpeg "$packing2
fi

temp=temp.change_packing.grib
rm -f $temp

# --- test changing the packing
# arg 1    : input grib file
# arg 2-n  : packing types to test
test_packing() {
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

# ECC-774
# ------------
input=${data_dir}/spherical_model_level.grib2
output=`${tools_dir}/grib_set -r -s packingType=spectral_simple $input $temp 2>&1`
# Check no error was posted i.e. output string is empty
[ -z "$output" ]
res1=`${tools_dir}/grib_get '-F%.1f' -p avg,enorm $input`
res2=`${tools_dir}/grib_get '-F%.1f' -p avg,enorm $temp`
[ "$res1" = "$res2" ]
rm -f $temp


# Change grib packing test
# -------------------------
test_packing $grib1 $packing1
test_packing $grib2 $packing2

rm -f $temp
