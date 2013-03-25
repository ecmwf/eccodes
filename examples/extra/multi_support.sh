#!/bin/sh

# test multi grib support, check if return value is good from grib_get

#set -xv

. ./include.sh

multi_file=${data_dir}/multi_created.grib2
multi_on=21
multi_off=1

if [ ! -f ${multi_file} ]; then
	echo "Data file missing: ${multi_file}"
	exit 1
fi

n=`${tools_dir}/grib_get -p count ${multi_file} | wc -l`
[ $n = ${multi_on} ]

n=`${tools_dir}/grib_get -M -p count ${multi_file} | wc -l`
[ $n = ${multi_off} ]
