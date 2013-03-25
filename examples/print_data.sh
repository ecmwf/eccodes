#!/bin/sh

. ./include.sh

numberOfValues=`${examples_dir}print_data ../data/constant_field.grib1 | grep values | awk '{print $1}'`

if [ $numberOfValues -ne 99200 ]
then
  echo ERROR: wrong number of values
  exit 1;
fi


