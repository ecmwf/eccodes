#!/bin/ksh
set -ex

decode=/var/tmp/mac/p4/bufrdc/releases/000402/examples/bufr_decode_data

for f in *.bufr
do 
  $decode -i $f | sed 's/MISSING/-1.00000000000000e+100/g' | sed -e '/ECMWF/,+8d' > $f.ref
done
