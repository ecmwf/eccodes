#!/usr/bin/ksh
set -ex

./list_all_keys.ksh


gperf -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  keys > ../src/grib_hash_keys.c

cat ../src/grib_itrie_keys.c | sed -e '/^#line /d' >> ../src/grib_hash_keys.c

