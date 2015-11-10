#!/bin/sh

. ./include.sh

#-----------------------------------------------------
# Create a BUFR from flight data
#----------------------------------------------------

bufr_in=${data_dir}"/bufr/syno_1.bufr"
csv_in="flight_data.csv"

bufr_out="temp.make_flight_bufr.bufr"

#fRef=$f".header.ref"
#fRes=$f".header.test.p"
REDIRECT=/dev/null

$PYTHON $examples_src/make_flight_bufr.py $csv_in $bufr_in $bufr_out

#We compare output to the reference by ignoring the whitespaces
#diff -w $fRef $fRes

#Clean up
rm -f $bufr_out
