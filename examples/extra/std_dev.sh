#!/bin/sh

# --- compute the standard deviation and compare the resulted values
# -     against reference values computed with mars

. ./include.sh

input=${data_dir}/small_ensemble.grib1
reference=${data_dir}/reference_stdev.grib1
temp=temp.grib1_

${examples_dir}/std_dev $input $temp
${tools_dir}/grib_compare -e 0.05 -c values $reference $temp > /dev/null

rm -f $temp || true
