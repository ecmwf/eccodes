#!/bin/sh

. ./include.sh

OUTPUT=grib_set_pv.py.temp.grib
$PYTHON $examples_src/grib_set_pv.py

x=`${tools_dir}/grib_get -p PVPresent,level,numberOfVerticalCoordinateValues $OUTPUT`
[ "$x" = "1 2 122" ]

rm $OUTPUT
