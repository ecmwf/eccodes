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

OUTPUT=all_codetable_flagtable_keys.txt
TEMP=temp.list_codetable_flagtable_keys.txt

[ -z "$ECCODES_DEFINITION_PATH" ] || ECCODES_DEFINITION_PATH=`${tools_dir}/codes_info -d`

touch $TEMP
echo "Go through all files in $ECCODES_DEFINITION_PATH ..."
for file in `find $ECCODES_DEFINITION_PATH/ -name '*.def' -print | grep -v grib3/`; do
  ${test_dir}/list_codetable_flagtable_keys $file >> $TEMP
done

sort -u < $TEMP > $OUTPUT
echo "Created $OUTPUT"
rm -f $TEMP
