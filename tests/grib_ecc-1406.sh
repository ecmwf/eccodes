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

label="grib_ecc-1406_test"

tempGrib=temp.$label.grib
tempFilt=temp.${label}.filt
tempOut=temp.${label}.txt

sample2="$samp_dir/GRIB2.tmpl"

cat > $tempFilt <<EOF
  set numberOfDataPoints = 5016591;
  set numberOfValues = 5016591;
  set Ni = 2801;
  set Nj = 1791;
  set latitudeOfFirstGridPoint = 55400000;
  set longitudeOfFirstGridPoint = 348000000;
  set latitudeOfLastGridPoint = 37500000;
  set longitudeOfLastGridPoint = 16000000;
  set iDirectionIncrement = 10000;
  set jDirectionIncrement = 10000;

  meta lastVal element(values, numberOfValues - 1);
  set lastVal = 42;

  write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample2
${tools_dir}/grib_ls -l 37.5,16.0,1 $tempGrib > $tempOut

grep -q "Grid Point chosen #1 index=5016590" $tempOut

${tools_dir}/grib_ls -j -l 37.5,16.0,1 $tempGrib > $tempOut
grep -q 'latitude" : 37.5, "longitude" : 16, "distance" : 0,.*"value" : 42 ,' $tempOut

rm -f $tempGrib $tempFilt $tempOut
