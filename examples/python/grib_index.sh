#!/bin/sh

. ./include.sh

TEMP1=temp1.grib_index.py.txt
TEMP2=temp2.grib_index.py.txt

IDX_FILE=my.idx
rm -f $IDX_FILE

# First time it will create the index file
$PYTHON $examples_src/grib_index.py > $TEMP1

[ -f "$IDX_FILE" ]

# Second time it will use the file created before
$PYTHON $examples_src/grib_index.py > $TEMP2

# Check output in both cases is the same
diff $TEMP1 $TEMP2

rm -f $IDX_FILE $TEMP1 $TEMP2
