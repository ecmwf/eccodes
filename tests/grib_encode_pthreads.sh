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

label="grib_encode_pthreads_test"

# Simple packing
# --------------
${test_dir}/grib_encode_pthreads $ECCODES_SAMPLES_PATH/GRIB2.tmpl
for f in temp.$label.out_*.grib; do
    ${tools_dir}/grib_get -p min,max,avg $f > $f.simple.txt
done

rm -f temp.$label.out_*.grib

if [ $HAVE_AEC -eq 1 ]; then
    # CCSDS packing
    # --------------
    ${test_dir}/grib_encode_pthreads $ECCODES_SAMPLES_PATH/ccsds_grib2.tmpl
    for f in temp.$label.out_*.grib; do
        ${tools_dir}/grib_get -p min,max,avg $f > $f.ccsds.txt
        diff $f.simple.txt $f.ccsds.txt
        rm $f.simple.txt $f.ccsds.txt
    done
fi

# Clean up
for f in temp.$label.out_*.grib; do
    rm -f $f.simple.txt $f.ccsds.txt temp.$label.out_*.grib
done
