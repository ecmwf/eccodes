#!/usr/bin/ksh
set -ex

./list_all_keys.sh


gperf -L C++ -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  keys > ../src/eccodes/grib_hash_keys.c

cat ../src/eccodes/grib_itrie_keys.c | sed -e '/^#line /d' >> ../src/eccodes/grib_hash_keys.c

