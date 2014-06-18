#!/usr/bin/ksh
set -x

cd ../tests

./list_all_keys.ksh

cd ../src

# Editing keys grib_hash_keys.c

gperf -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  ../tests/keys |\
  sed '/__attribute__ ((__gnu_inline__))/d' |\
  sed s/__inline//g | sed s/inline//g  > grib_hash_keys.c

cat grib_itrie_keys.c >> grib_hash_keys.c

