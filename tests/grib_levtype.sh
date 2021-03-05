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
label="grib_levtype"

if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
    echo "Test $0 disabled. No definitions directory"
    exit 0
fi

sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
tempGrib=temp.${label}.grib
latestAvailable=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`
latestOfficial=`${tools_dir}/grib_get -p tablesVersionLatestOfficial $sample2`

# These level types are S2S ocean parameters and are dealt with differently (See products_s2s.def)
exclude="20 160 169"

paramId_file_wmo="$ECCODES_DEFINITION_PATH/grib2/paramId.def"
paramId_file_ecm="$ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf/paramId.def"

levtypes=`grep typeOfFirstFixedSurface $paramId_file_wmo $paramId_file_ecm |\
         awk -F'=' '{print $2}'|tr -d ' '|tr -d ';'|sort -un`
for lt in $levtypes; do
    process_type=1
    for ex in $exclude; do
        if [ "$lt" = "$ex" ]; then process_type=0; break; fi
    done
    if [ $process_type = 1 ]; then
        ${tools_dir}/grib_set -s tablesVersion=$latestAvailable,typeOfFirstFixedSurface=$lt $sample2 $tempGrib
        result=`${tools_dir}/grib_get -p mars.levtype $tempGrib`
        if [ "$result" = "$lt" ]; then
            echo "ERROR: typeOfFirstFixedSurface of |$lt| not mapped to a string!"
            exit 1
        fi
        ${tools_dir}/grib_set -s tablesVersion=$latestOfficial,typeOfFirstFixedSurface=$lt $sample2 $tempGrib
        result=`${tools_dir}/grib_get -p mars.levtype $tempGrib`
        if [ "$result" = "$lt" ]; then
            echo "ERROR: typeOfFirstFixedSurface of |$lt| not mapped to a string!"
            exit 1
        fi
    fi
done

rm -f $tempGrib
