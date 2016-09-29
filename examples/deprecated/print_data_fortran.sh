#!/bin/sh

. ./include.sh

numberOfValues=`${examples_dir}print_data_fortran | grep values | awk '{print $1}'`

if [ $numberOfValues -ne 99200 ]
then
  echo ERROR: wrong number of values
  exit 1;
fi


