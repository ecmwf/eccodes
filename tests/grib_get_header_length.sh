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

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempTextA=temp.A.$label.txt
tempTextB=temp.B.$label.txt
tempFilt=temp.$label.filt

inputs="
  sample.grib2
  jpeg.grib2
  reduced_gaussian_model_level.grib2
  test_file.grib2
  ccsds.grib2
  tigge_af_ecmwf.grib2
"

cat >$tempFilt<<EOF
 if (count == 1) {
   assert(edition == 2);
   if (defined(section2Length)) {
     transient tot = section0Length + section1Length + section2Length + section3Length + section4Length;
   } else {
     transient tot = section0Length + section1Length + section3Length + section4Length;
   }
   print "[tot]";
 }
EOF

for f in $inputs; do
    infile=$data_dir/$f
    $EXEC ${test_dir}/grib_get_header_length $infile > $tempTextA
    ${tools_dir}/grib_filter $tempFilt $infile       > $tempTextB
    diff $tempTextA $tempTextB
done

# Clean up
rm -f $tempTextA $tempTextB $tempFilt
