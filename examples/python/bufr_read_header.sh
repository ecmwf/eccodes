#!/bin/sh

. ./include.sh

#-----------------------------------------------------
# Test reading the header only from a BUFR
# file with multiple messages
#----------------------------------------------------

#The bufr filename is hardcoded in the python example
f=${data_dir}"/bufr/syno_multi.bufr"
fRef=$f".header.ref"
fRes=$f".header.test.p"
REDIRECT=/dev/null

#Write the values into a file and compare with reference
$PYTHON $examples_src/bufr_read_header.py 2> $REDIRECT > $fRes

#We compare output to the reference by ignoring the whitespaces
diff -w $fRef $fRes

#cat $fRes

#Clean up
rm -f $fRes
