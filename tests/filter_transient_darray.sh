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

label="filter_transient_darray_test"

tempOut=temp.$label.out
tempRef=temp.$label.ref
tempFilt=temp.$label.filt
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat > $tempFilt <<EOF
    transient dar = { 6, -1.6 };
    print "dar as int=[dar:i]";
    print "dar as dbl=[dar:d]";
    set dar = {-9.99, 14.76, 15, 16, 18};
    print "dar now =[dar]";
    meta elem1 element(dar, 1);
    print "elem1 = [elem1:d]";
EOF

${tools_dir}/grib_filter $tempFilt $sample > $tempOut

cat > $tempRef <<EOF
dar as int=6 -1
dar as dbl=6 -1.6
dar now =-9.99 14.76 15 16 18
elem1 = 14.76
EOF

diff $tempRef $tempOut


# Clean up
rm -f $tempRef $tempOut $tempFilt
