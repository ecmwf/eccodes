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
set -u
REDIRECT=/dev/null

label="grib2_version"
if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
    echo "Test $0 disabled. No definitions directory"
    exit 0
fi

temp=temp.$label.grib2
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
tables_dir="$ECCODES_DEFINITION_PATH/grib2/tables"

# In $tables_dir, the directory names should match the version numbers of GRIB2 tables.
# Check the directory with the highest number matches the key 'tablesVersionLatest'
cd $tables_dir
highest_num=`ls -1d [0-9]* | sort -rn | sed 1q`
latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`
if [ "$latest" != "$highest_num" ]; then
    echo "The GRIB2 key tablesVersionLatest = $latest but the highest number in $tables_dir is $highest_num"
    exit 1
fi

# Also grib1 to grib2 conversion should set the official version, not the highest
${tools_dir}/grib_set -s edition=2 $sample1 $temp
tablesVersion=`${tools_dir}/grib_get -p tablesVersion $temp`
latestOfficial=`${tools_dir}/grib_get -p tablesVersionLatestOfficial $temp`
if [ "$tablesVersion" != "$latestOfficial" ]; then
    echo "After conversion to GRIB2, tablesVersion=$tablesVersion. Should be $latestOfficial"
    exit 1
fi

rm -f $temp
