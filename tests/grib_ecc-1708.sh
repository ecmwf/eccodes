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

label="grib_ecc-1708_test"

tempGrib=temp.$label.grib
tempFilt=temp.${label}.filt
tempOut=temp.${label}.txt

sample="$samp_dir/GRIB1.tmpl"

cat > $tempFilt <<EOF
  set Ni = 1401;
  set Nj = 701;
  set latitudeOfFirstGridPoint  = 80000;
  set longitudeOfFirstGridPoint = -180000;
  set latitudeOfLastGridPoint   = 10000;
  set longitudeOfLastGridPoint  = -40000;
  set iDirectionIncrement = 100;
  set jDirectionIncrement = 100;
  write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample
${tools_dir}/grib_ls -j -n geography $tempGrib

cat > $tempFilt <<EOF
  meta last_elem element(distinctLongitudes, 1400);
  # print "[last_elem:d]";
  if ( last_elem + 40 > 0.001 ) {
    print "Error: last longitude is [last_elem:d] but should be -40";
    assert(0);
  }
EOF
${tools_dir}/grib_filter $tempFilt $tempGrib

rm -f $tempGrib $tempFilt $tempOut
