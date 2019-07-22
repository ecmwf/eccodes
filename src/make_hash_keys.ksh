#!/usr/bin/ksh
set -xe

cd ../tests

echo "List all keys..."
./list_all_keys.ksh

cd ../src

# Editing keys grib_hash_keys.c

gperf -l -C -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  ../tests/keys |\
  sed '/__attribute__ ((__gnu_inline__))/d' |\
  sed s/__inline//g | sed s/inline//g  | sed -e '/^#line /d' > grib_hash_keys.c

cat grib_itrie_keys.c >> grib_hash_keys.c

