#!/bin/sh

. ./include.sh

#-----------------------------------------------------
# Create a BUFR from flight data
#----------------------------------------------------
label="bufr_encode_flight_p"

bufr_in=${data_dir}"/bufr/syno_1.bufr"
csv_in="flight_data.csv"

fBufrTmp=${label}.tmp.bufr

REDIRECT=/dev/null

$PYTHON $examples_src/bufr_encode_flight.py $csv_in $bufr_in $fBufrTmp

# Make sure output is OK
${tools_dir}bufr_dump $fBufrTmp > $REDIRECT

numSubsets=`${tools_dir}bufr_get -p numberOfSubsets $fBufrTmp`
[ "$numSubsets" = "185" ]

rm -f $fBufrTmp
