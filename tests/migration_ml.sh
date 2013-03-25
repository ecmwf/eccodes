#!/bin/sh

# --- setup the environment
. ./include.sh

src=${data_dir}/essential_ml/grib1_ecmwf
dest=${data_dir}/essential_ml/grib2_ecmwf

if [ ! -d $src ]; then
    echo "Data source folder does not exist: '$src'"
    exit 1
fi

mkdir -p $dest

# --- convert to grib 2

for file in `ls $src/*.grib`; do
    f=`basename $file`
    ${tools_dir}/grib_set -s edition=2 $file $dest/$f
    
    if [ ! $? == 0 ]; then
        echo "FAIL"
        exit 1
    fi
done

# --- compare the results

set +e # don't fail on error for now
for file in `ls $src/*.grib`; do
    name=`basename $file`
    echo "Comparing $name"
    ${tools_dir}/grib_compare -c mars:n,name,shortName $src/$name $dest/$name
done
set -e
