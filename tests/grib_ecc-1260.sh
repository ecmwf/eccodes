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

label="grib_ecc-1260_test"
tempGrib=temp.$label.grib
tempRef=temp.$label.ref
tempOut=temp.$label.txt

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a GRIB with a stepRange having a hyphen
${tools_dir}/grib_set -s stepType=avg,productionStatusOfProcessedData=6,typeOfProcessedData=3,stepRange=0-24 \
   $sample_grib2 $tempGrib
grib_check_key_equals $tempGrib step,stepRange "0-24 0-24"

# Put the average first
${tools_dir}/grib_get -p step $tempGrib $sample_grib2 > $tempOut
cat > $tempRef << EOF
0-24
0
EOF
diff $tempRef $tempOut

# Put the instantaneous first
${tools_dir}/grib_get -p step $sample_grib2 $tempGrib > $tempOut
cat > $tempRef << EOF
0
0-24
EOF
diff $tempRef $tempOut


# Clean up
rm -f $tempRef $tempGrib $tempOut
