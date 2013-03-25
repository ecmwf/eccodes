#!/usr/bin/ksh
set -x

cd ../tests

./list_all_keys.ksh

cd ../src

p4 edit keys grib_hash_keys.c

gperf -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  ../tests/keys | sed s/__inline//g | sed s/inline//g  > grib_hash_keys.c

cat grib_itrie_keys.c >> grib_hash_keys.c

