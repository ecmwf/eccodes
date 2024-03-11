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

label="grib_suppressed_test"

tempErr=temp.${label}.err
tempFilt=temp.${label}.filt
tempRef=temp.${label}.ref
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempRef <<EOF
ECCODES ERROR   :  key 'a_deprecated_key' is unavailable in this version.
ECCODES ERROR   :  Please use the following key(s):
ECCODES ERROR   :  	- typeOfOriginalFieldValues
ERROR: Key/value not found (message 1)
EOF

do_tests()
{
  qualifier=$1
  echo "meta a_deprecated_key suppressed(typeOfOriginalFieldValues);  print \"[a_deprecated_key$qualifier]\";" > $tempFilt
  set +e
  ${tools_dir}/grib_filter $tempFilt $sample2  2>$tempErr
  status=$?
  set -e
  [ $status -ne 0 ]
  cat $tempErr
  diff $tempRef $tempErr
}

do_tests ""
do_tests ":i"
do_tests ":d"


# Clean up
rm -f $tempFilt $tempErr $tempRef
