#!/bin/sh
set -xe

cd ../tests

echo "List all keys..."
./list_all_keys.ksh

cd ../src

# Editing keys grib_hash_keys.c
# -l  Compare key lengths before trying a string comparison
# -C  Make the contents of generated lookup tables constant, i.e., readonly
# -I  Include the necessary system include file <string.h> at the beginning of the code
# -t  Allows the user to include a structured type declaration for generated code
# -G  Generate  the static table of keywords as a static global variable
# -H  Specify name of generated hash function
# -N  Specify name of generated lookup function

gperf -l -C -I -t -G -H hash_keys -N grib_keys_hash_get -m 3  ../tests/keys |\
  sed -e '/^#line /d' > grib_hash_keys.c

cat grib_itrie_keys.c >> grib_hash_keys.c

echo ALL DONE
