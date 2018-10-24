#!/bin/sh

. ./include.sh

#-----------------------------------------------------
# Test reading the header from a BUFR
# file with multiple messages
#----------------------------------------------------

f=${data_dir}/bufr/syno_multi.bufr
fRef=$f".header.ref"
fRes=$f".header.test.p"
REDIRECT=/dev/null

# Write the values into a file and compare with reference
$PYTHON $examples_src/bufr_read_header.py $f 2> $REDIRECT > $fRes

#We compare output to the reference by ignoring the whitespaces
diff -w $fRef $fRes

# Now test reading the header of sample files
$PYTHON $examples_src/bufr_read_header.py ${ECCODES_SAMPLES_PATH}/BUFR3_local.tmpl
$PYTHON $examples_src/bufr_read_header.py ${ECCODES_SAMPLES_PATH}/BUFR4.tmpl
$PYTHON $examples_src/bufr_read_header.py ${ECCODES_SAMPLES_PATH}/BUFR4_local_satellite.tmpl


#Clean up
rm -f $fRes
