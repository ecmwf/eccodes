#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -ex

. ./include.sh

REDIRECT=/dev/null

# Decide if we have the JSON verifier commandline utility
JSON_VERIF="json_xs"
JSON_CHECK=""
if command -v $JSON_VERIF >/dev/null 2>&1; then
  JSON_CHECK=$JSON_VERIF
fi

for file in ${data_dir}/bufr/*.bufr
do
  rm -f ${file}.json | true

  ${tools_dir}bufr_dump -j $file 2> $REDIRECT > ${file}.json

  if test "x$JSON_CHECK" != "x"; then
    json_xs < ${file}.json >$REDIRECT 2> $REDIRECT
  fi

  rm -f ${file}.json

  ${tools_dir}bufr_dump -jL $file 2> $REDIRECT > ${file}.json

  if test "x$JSON_CHECK" != "x"; then
    json_xs < ${file}.json >$REDIRECT 2> $REDIRECT
  fi

  rm -f ${file}.json
done
