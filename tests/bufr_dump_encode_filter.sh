#!/bin/sh
# Copyright 2005-2018 ECMWF.
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

# Exclude BUFR files for various reasosn:
# ias1_240.bufr: Too large. The filter rules file generated is too big for the parser!
# tropical_cyclone.bufr: multi-message
# syno_multi.bufr: multi-message
# israel_observations_2017041010.bufr: Suffers from a bug. In filter cannot do:
#    set #1#3HourPressureChange=-1.6;
# The hash cannot be followed by a number!
exclude="ias1_240.bufr syno_multi.bufr tropical_cyclone.bufr israel_observations_2017041010.bufr"

for f in $files
do
  process_bufr=1
  for ex in $exclude; do
    if [ "$f" = "$ex" ]; then process_bufr=0; break; fi
  done
  if [ $process_bufr = 1 ]; then
    echo "Test: bufr_dump -Efilter " >> $fLog
    echo "file: $f" >> $fLog

    ${tools_dir}/bufr_dump -Efilter $f > $fRules

    ${tools_dir}/codes_bufr_filter -o $fBufrTmp $fRules $f
    ${tools_dir}/bufr_compare $fBufrTmp $f

    TEMP_OUT1=${label}.$f.dump.out
    TEMP_OUT2=${label}.$fBufrTmp.dump.out
    ${tools_dir}/bufr_dump -p $f        > $TEMP_OUT1
    ${tools_dir}/bufr_dump -p $fBufrTmp > $TEMP_OUT2
    diff $TEMP_OUT1 $TEMP_OUT2
    rm -f $TEMP_OUT1 $TEMP_OUT2

    rm -f $fBufrTmp $fRules
  fi
done
rm -f $fLog $fBufrTmp $fRules
