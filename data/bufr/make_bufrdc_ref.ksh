#!/bin/ksh
set -ex

base=/var/tmp/mac/p4/bufrdc/releases/000402/examples
decode=$base/bufr_decode_data
print_descriptors=$base/bufr_print_descriptors

for f in synop_multi_subset.bufr
do 
  $decode -i $f | sed 's/MISSING/-1.00000000000000e+100/g' | sed -e '/ECMWF/,+8d' > $f.num.ref
  $print_descriptors -i $f | sed -e '/ECMWF/,+8d'| sed 's/ //g' > $f.desc.ref
done
