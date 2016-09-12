#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -x
. ./include.sh

cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_dump_encode_filter_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# NOTE: not all of our BUFR files pass this test. bufr_filter is limited
# in what it can do compared to Python or Fortran!
# The following do not work:
#   ias1_240.bufr           -- too large, parser out of memory
#   tropical_cyclone.bufr   -- multi message
#   syno_multi.bufr         -- multi message
#-----------------------------------------------------------
files=`cat ${data_dir}/bufr/bufr_data_files.txt`

exclude="ias1_240.bufr syno_multi.bufr tropical_cyclone.bufr"

for f in $files
do
  process_bufr=1
  for ex in $exclude; do
    if [ "$f" = "$ex" ]; then process_bufr=0; break; fi
  done
  if [ $process_bufr = 1 ]; then
    echo "Test: bufr_dump -Efilter " >> $fLog
    echo "file: $f" >> $fLog

    ${tools_dir}bufr_dump -Efilter $f > $fRules

    ${tools_dir}bufr_filter -o $fBufrTmp $fRules $f
    ${tools_dir}bufr_compare $fBufrTmp $f

    TEMP_JSON1=${label}.$f.json
    TEMP_JSON2=${label}.$fBufrTmp.json
    ${tools_dir}bufr_dump $f        > $TEMP_JSON1
    ${tools_dir}bufr_dump $fBufrTmp > $TEMP_JSON2
    diff $TEMP_JSON1 $TEMP_JSON2
    rm -f $TEMP_JSON1 $TEMP_JSON2

    rm -f $fBufrTmp $fRules
  fi
done
rm -f $fLog $fBufrTmp $fRules
