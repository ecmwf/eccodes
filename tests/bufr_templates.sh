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

label="bufr_templates_test"

temp=${label}.bufr
sample=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

templates_file="${ECCODES_DEFINITION_PATH}/bufr/templates/BufrTemplate.def"

# TODO: There is a matching issue with OmpsNadirProfile: mixed up with OmpsTotalColumn.
#       So exclude that one
templates=`cat $templates_file | awk -F= '{print $1}' | tr -d '"' | grep -v OmpsNadirProfile`


# Note: bufrTemplate and BufrTemplate are the same key
for t in $templates; do
    ${tools_dir}/bufr_set -s bufrTemplate=$t $sample $temp
    res=`${tools_dir}/bufr_get -p BufrTemplate $temp`
    [ "$res" = "$t" ]
    ${tools_dir}/bufr_dump -p $temp > /dev/null
done

rm -f $temp
