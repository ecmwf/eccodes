#!/bin/sh

# test cloning is done properly

#set -xv

. ./include.sh

temp="${data_dir}/temp_clone.grib"

for file in `ls ${data_dir}/*.grib?`; do
	${examples_dir}/f_clone $file $temp

	set +e
	${tools_dir}/grib_compare -f $file ${temp} &> /dev/null

	if [ ! $? -eq 0 ]; then
		false
		exit
	fi
	set -e
done

rm -f $temp || true
