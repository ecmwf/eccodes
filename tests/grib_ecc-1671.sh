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

label="grib_ecc-1671_test"

tempGrib=temp.$label.grib
tempFilt=temp.${label}.filt
tempOut=temp.${label}.txt

sample="$samp_dir/GRIB2.tmpl"

cat > $tempFilt <<EOF
  set numberOfDataPoints = 5400000;
  set numberOfValues = 5400000;
  set Ni = 3600;
  set Nj = 1500;
  set latitudeOfFirstGridPoint = 89950000;
  set longitudeOfFirstGridPoint = 180050000;
  set latitudeOfLastGridPoint = -59950000;
  set longitudeOfLastGridPoint = 539950000;
  set iDirectionIncrement = 100000;
  set jDirectionIncrement = 100000;
  write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample
# ${tools_dir}/grib_ls -j -n geography $tempGrib

cat > $tempFilt <<EOF
  meta last_elem element(distinctLongitudes, 3599);
  # print "[last_elem:d]";
  if ( last_elem - 179.95 > 0.001 ) {
    print "Error: last longitude is [last_elem:d] but should be 179.95";
    assert(0);
  }
EOF
${tools_dir}/grib_filter $tempFilt $tempGrib

# ${tools_dir}/grib_get_data $tempGrib > $tempOut
# cat $tempOut
# ${tools_dir}/grib_ls -l 37.5,16.0,1 $tempGrib > $tempOut

rm -f $tempGrib $tempFilt $tempOut
