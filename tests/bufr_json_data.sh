#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

label="bufr_json_data_test"
tempRules=temp.${label}.filter
tempOut=temp.${label}.txt
tempBufr=temp.${label}.bufr


if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi


REDIRECT=/dev/null

cd ${data_dir}/bufr

if [ $HAVE_MEMFS -eq 1 ]; then
    unset ECCODES_DEFINITION_PATH
    unset ECCODES_SAMPLES_PATH
fi

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi
# ECC-1119: Check the json_xs command actually works!
set +e
echo '[]' | json_xs > $REDIRECT 2>&1
if [ $? -ne 0 ]; then JSON_CHECK=""; fi
set -e
echo "Using $JSON_CHECK ..."

# Test downloaded BUFR files
# -------------------------
bufr_files=`cat bufr_data_files.txt`
for file in ${bufr_files}; do
  # Test the various JSON dump modes: 'structure', 'all' and 'flat'
  for mode in s a f; do
    rm -f ${file}.json
    ${tools_dir}/bufr_dump -j$mode $file 2> $REDIRECT > ${file}.json
    if test "x$JSON_CHECK" != "x"; then
      json_xs < ${file}.json >$REDIRECT 2> $REDIRECT
    fi
  done

  rm -f ${file}.json
done

# ECC-233: Test JSON dump when selecting messages with '-w' switch
# -----------------------------------------------------------------
file=tropical_cyclone.bufr
for c in 1 3 1/3; do
  ${tools_dir}/bufr_dump -w count=$c $file 2> $REDIRECT > ${file}.json
  if test "x$JSON_CHECK" != "x"; then
    json_xs < ${file}.json >$REDIRECT 2> $REDIRECT
  fi
  rm -f ${file}.json
done


# ECC-272
# ------------
file=aaen_55.bufr
${tools_dir}/bufr_dump -jf $file | grep -q -w channelRadiance


# ECC-1401: BUFR: bufr_dump generates invalid JSON
# -------------------------------------------------
cat > $tempRules <<EOF
    set unexpandedDescriptors = { 1015 };
    set stationOrSiteName = 'Contains "bad" chars';
    set pack = 1;
    write;
EOF
${tools_dir}/codes_bufr_filter -o $tempBufr $tempRules $samp_dir/BUFR4.tmpl
${tools_dir}/bufr_dump $tempBufr > $tempOut
if test "x$JSON_CHECK" != "x"; then
    json_xs -t none < $tempOut
fi


rm -f $tempRules $tempOut $tempBufr
