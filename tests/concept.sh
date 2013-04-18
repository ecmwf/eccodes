#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

data="${data_dir}/*grib1 ${data_dir}/*grib2"
tmp="tmp.out"

for d in $data
do

set -e
${tools_dir}grib_ls -p gridType,packingType -w dataRepresentationType!=192 $d > $tmp

set +e
grep unknown $tmp

if [ $? -eq 0 ] 
then 
  false
  exit
fi

rm -f $tmp | true

done

