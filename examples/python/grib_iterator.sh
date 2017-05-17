#!/bin/sh

. ./include.sh

label="py_grib_iterator"
TEMP1=$label.temp1.$$
TEMP2=$label.temp2.$$
TEMP3=$label.temp3.$$

# Input file with some missing data values
FILE="${data_dir}/reduced_latlon_surface.grib1"
$PYTHON $examples_src/grib_iterator.py $FILE 2> $TEMP1 > $TEMP1
./p_grib_iterator $FILE                      2> $TEMP2 > $TEMP2
diff $TEMP1 $TEMP2 >/dev/null

$PYTHON $examples_src/grib_iterator_bitmap.py $FILE 2> $TEMP3 > $TEMP3
diff $TEMP1 $TEMP3 >/dev/null


# Polar stereographic
FILE="${ECCODES_SAMPLES_PATH}/polar_stereographic_pl_grib2.tmpl"
$PYTHON $examples_src/grib_iterator.py $FILE 2> $TEMP1 > $TEMP1
./p_grib_iterator $FILE        2> $TEMP2 > $TEMP2
diff $TEMP1 $TEMP2 >/dev/null

$PYTHON $examples_src/grib_iterator_bitmap.py $FILE 2> $TEMP3 > $TEMP3
diff $TEMP1 $TEMP3 >/dev/null

# Clean up
rm $TEMP1 $TEMP2 $TEMP3
