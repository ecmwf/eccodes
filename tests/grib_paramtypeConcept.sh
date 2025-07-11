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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_paramtypeconcept_all_pdtn"  # Change prod to bufr or grib etc
tempGrib=temp.$label.grib
tempText=temp.$label.txt
rm -f $tempGrib $tempText

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
latestOfficial=$( ${tools_dir}/grib_get -p tablesVersionLatestOfficial $sample_grib2 )
latest_codetable_file=$ECCODES_DEFINITION_PATH/grib2/tables/$latestOfficial/4.0.table
if [ -f "$latest_codetable_file" ]; then
  pdtns=$( awk '$1 !~ /#/ && $1 < 65000 {print $1}' $latest_codetable_file )
  for pdtn in $pdtns; do
    # echo "Doing $pdtn "
    ${tools_dir}/grib_set -s tablesVersion=$latestOfficial,productDefinitionTemplateNumber=$pdtn $sample_grib2 $tempGrib
    ${tools_dir}/grib_get -p paramtype $tempGrib
    paramtype=$( ${tools_dir}/grib_get -p paramtype $tempGrib )
    if [[ $paramtype  == 'unknown' ]]; then
      echo "productDefinitionTemplateNumber=$pdtn: paramtype not defined"
      exit 1
    fi
  done
fi

# Clean up
rm -f $tempGrib $tempText 
