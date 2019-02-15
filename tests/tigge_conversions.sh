#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# --- test grib edition 2 to 1 conversions with tigge data
# ---   using the new features of grib_compare that allows
# ---   namespace comparison

. ./include.sh

REDIRECT=/dev/null

dir="${data_dir}/tigge/"
temp1="temp.grib1_"
temp2="temp.grib2_"

# --- Do I want to exclude any file pattern from the comparison ?
exclusion_pattern="tcw|ssr|str|skt|cap|ci|ttr|st|sm|sd|slhf|sshf"

for file in ${dir}/tigge_*.grib; do
  exclude=`echo $file | awk " /$exclusion_pattern/ {print \"found\";} "`
  if [ -z "$exclude" ]; then
    rm -f ${temp1} ${temp2}

    # 2 to 1 conversion check
    ${tools_dir}/grib_set -s editionNumber=1 ${file} ${temp1} 2> $REDIRECT > $REDIRECT
    ${tools_dir}/grib_compare -P -c data:n,geography:n ${temp1} ${file} 2> $REDIRECT > $REDIRECT

    # 1 to 2 conversion check
    ${tools_dir}/grib_set -s editionNumber=2 ${temp1} ${temp2} 2> $REDIRECT > $REDIRECT
    ${tools_dir}/grib_compare -P -c shortName,data:n,geography:n ${temp2} ${file} 2> $REDIRECT > $REDIRECT
  fi
done

rm -f ${temp1} ${temp2}
