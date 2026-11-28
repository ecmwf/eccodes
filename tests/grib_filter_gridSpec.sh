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

# Strings longer than 1024 characters by the filter, of keys (gridSpec, of a pl array): transient, set, compare and print

if [ $HAVE_GEOGRAPHY -ne 1 ] || [ $HAVE_ECKIT_GEO -ne 1 ]; then
    echo "$0: This test is disabled when HAVE_GEOGRAPHY=OFF or HAVE_ECKIT_GEO=OFF"
    exit 0
fi
export ECCODES_ECKIT_GEO=1
set -u

label="grib_filter_gridSpec_test"
tempFilt=temp.$label.filt
tempText=temp.$label.txt

infile=$ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib2.tmpl
gridSpec=$(${tools_dir}/grib_get -p gridSpec $infile)
[ ${#gridSpec} -gt 1024 ]

cat >$tempFilt<<EOF2
    transient gs1 = gridSpec;
    set gridSpec = gs1;
    transient gs2 = gridSpec;
    assert( gs1 is gs2 );
    print "[gs2]";
EOF2
${tools_dir}/grib_filter $tempFilt $infile > $tempText
[ "$(cat $tempText)" = "$gridSpec" ]

# Clean up
rm -f $tempFilt $tempText
