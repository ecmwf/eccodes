#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
set -e

if [ -z "$ECCODES_DEFINITION_PATH" ]; then
  ECCODES_DEFINITION_PATH=`${tools_dir}/codes_info -d`
fi

GRIB_LIST_KEYS=${tools_dir}/grib_list_keys
if [ ! -x $GRIB_LIST_KEYS ]; then
  # Get it from environment variable
  GRIB_LIST_KEYS=$GRIB_LIST_KEYS_EXE
fi

touch tmp$$
for file in `find $ECCODES_DEFINITION_PATH -name '*.def' -print`
do
  ${GRIB_LIST_KEYS} $file >> tmp$$  
done

cat >keys <<EOF
%{
#include "grib_api_internal.h"
%}
struct grib_keys_hash { char* name; int id;};
%%
EOF
cat tmp$$ | sort | uniq | awk 'BEGIN{x=0;}{print $1","++x}' >> keys
#cat tmp$$ | sort | uniq  >> keys
rm -f tmp$$
