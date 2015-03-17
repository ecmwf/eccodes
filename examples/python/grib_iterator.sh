#!/bin/sh

. ./include.sh

TEMP1=temp1.$$
TEMP2=temp2.$$

FILE="${data_dir}/reduced_latlon_surface.grib1"
$PYTHON grib_iterator.py $FILE 2> $TEMP1 > $TEMP1
./p_grib_iterator $FILE        2> $TEMP2 > $TEMP2
diff $TEMP1 $TEMP2


# Polar stereographic
FILE="${ECCODES_SAMPLES_PATH}/polar_stereographic_pl_grib2.tmpl"
$PYTHON grib_iterator.py $FILE 2> $TEMP1 > $TEMP1
./p_grib_iterator $FILE        2> $TEMP2 > $TEMP2
diff $TEMP1 $TEMP2


rm $TEMP1 $TEMP2 || true
