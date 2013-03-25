#!/bin/sh

. ./include.sh

if [ ! -f "${data_dir}/multi.grib2" ] 
then
  echo SKIP: $0
  exit
fi
${examples_dir}multi > /dev/null


