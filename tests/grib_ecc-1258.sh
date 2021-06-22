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

label="grib_ecc-1258-test"
tempFilt=temp.$label.filt
tempGrib=temp.$label.grib
tempOut=temp.$label.txt
sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

cat > $tempFilt <<EOF
 set Ni = 16;
 set Nj = 21;
 set latitudeOfFirstGridPoint= -29600;
 set longitudeOfFirstGridPoint = 134000;
 set latitudeOfLastGridPoint= -37600;
 set longitudeOfLastGridPoint= 140000;
 set iDirectionIncrement = 400;
 set jDirectionIncrement= 400;
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib1
${tools_dir}/grib_ls  -l -32.4,137.6  $tempGrib > $tempOut
grep -q "Grid Point chosen #3 index=121" $tempOut
grep -q "index=121 .*distance=0.0" $tempOut


rm -f $tempFilt $tempGrib $tempOut
