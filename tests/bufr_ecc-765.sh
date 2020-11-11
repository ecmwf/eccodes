#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-765.
# BUFR decoding memory leaks
# ---------------------------------------------------------
label="bufr_ecc-765-test"
tempLog=temp.${label}.log
tempSupp=temp.${label}.supp
tempFilt=temp.${label}.filt

if test "x$ECCODES_TEST_WITH_VALGRIND" = "x"; then
  echo "Environment variable ECCODES_TEST_WITH_VALGRIND not defined. Test disabled"
  exit 0
fi

# The presence of ECCODES_TEST_WITH_VALGRIND environment variable redefines
# tools_dir so we reset it to its original
tools_dir=$build_dir/bin

files=`cat ${data_dir}/bufr/bufr_data_files.txt`
VALGRIND_OPTIONS="--error-exitcode=1 --leak-check=full --log-file=$tempLog"

cat > $tempFilt <<EOF
  set unpack=1;
EOF

for bf in $files; do
  valgrind $VALGRIND_OPTIONS ${tools_dir}/codes_bufr_filter $tempFilt ${data_dir}/bufr/$bf >/dev/null
done


# Suppressions file
# ------------------
#cat > $tempSupp << EOF
#{
#   eccodes_string_split_suppression
#   Memcheck:Leak
#   match-leak-kinds: definite
#   fun:malloc
#   fun:string_split
#   fun:load_bufr_elements_table*
#   fun:bufr_get_from_table
#   fun:accessor_bufr_elements_table_get_descriptor
#   fun:grib_bufr_descriptor_new
#}
#EOF
#VALGRIND_OPTIONS="--error-exitcode=1 --leak-check=full --log-file=$tempLog --suppressions=$tempSupp "


# Clean up
rm -f $tempLog $tempSupp $tempFilt
