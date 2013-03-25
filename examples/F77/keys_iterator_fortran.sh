#!/bin/sh

. ./include.sh
cat > test.txt <<EOF
-- GRIB N.   1 --
 |edition| = |1|
 |centre| = |ecmf|
 |param| = |167.128|
 |levelType| = |sfc|
 |lev| = |0|
 |date| = |20080206|
 |stepUnits| = |h|
 |stepRange| = |0|
 |dataType| = |an|
 |shortName| = |2t|
 |valuesCount| = |496|
 |packingType| = |grid_simple|
 |gridType| = |regular_ll|
EOF

${examples_dir}keys_iterator_fortran > test_fortran.txt

diff test_fortran.txt test.txt

#rm -f test_fortran.txt test.txt


